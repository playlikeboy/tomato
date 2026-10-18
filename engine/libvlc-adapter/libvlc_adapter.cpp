#include "engine/libvlc-adapter/libvlc_adapter.h"
#include <vlc/vlc.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

namespace tomato::engine {
namespace {
int64_t millisecondsToMicroseconds(int64_t ms) {
    constexpr int64_t scale = 1000;
    if (ms > std::numeric_limits<int64_t>::max() / scale) return std::numeric_limits<int64_t>::max();
    if (ms < std::numeric_limits<int64_t>::min() / scale) return std::numeric_limits<int64_t>::min();
    return ms * scale;
}
bool hasUriScheme(const std::string& value) {
    const auto separator = value.find("://");
    return separator != std::string::npos && separator > 0;
}
std::string localPathToFileUri(const std::string& input) {
    std::error_code error;
    std::filesystem::path path(input);
    if (path.is_relative()) {
        auto absolute = std::filesystem::absolute(path, error);
        if (!error) path = std::move(absolute);
    }
    std::ostringstream encoded;
    encoded << std::uppercase << std::hex;
    for (unsigned char ch : path.generic_string()) {
        const bool safe = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') ||
            (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' ||
            ch == '~' || ch == '/' || ch == ':';
        if (safe) encoded << static_cast<char>(ch);
        else encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(ch);
    }
#if defined(_WIN32)
    return "file:///" + encoded.str();
#else
    return "file://" + encoded.str();
#endif
}
} // namespace

LibVLCAdapter::LibVLCAdapter() { initialize(); }
LibVLCAdapter::~LibVLCAdapter() { cleanup(); }

bool LibVLCAdapter::initialize(const std::vector<std::string>& customArgs) {
    if (m_instance && m_player) return true;
    cleanup();
    std::vector<const char*> args{"--no-video-title-show", "--no-snapshot-preview", "--quiet"};
    args.reserve(args.size() + customArgs.size());
    for (const auto& value : customArgs) args.push_back(value.c_str());
    m_instance = libvlc_new(static_cast<int>(args.size()), args.data());
    if (!m_instance) {
        std::cerr << "[Tomato] LibVLC initialization failed; check its runtime and plugins.\n";
        return false;
    }
    m_player = libvlc_media_player_new(m_instance);
    if (!m_player) {
        libvlc_release(m_instance);
        m_instance = nullptr;
        std::cerr << "[Tomato] Could not create the LibVLC media player.\n";
        return false;
    }
    setupEventListeners();
    if (m_windowHandle) attachWindow(m_windowHandle);
    return true;
}

void LibVLCAdapter::cleanup() {
    if (m_player) {
        libvlc_media_player_stop(m_player);
        libvlc_media_player_release(m_player);
        m_player = nullptr;
    }
    if (m_currentMedia) {
        libvlc_media_release(m_currentMedia);
        m_currentMedia = nullptr;
    }
    if (m_instance) {
        libvlc_release(m_instance);
        m_instance = nullptr;
    }
}

void LibVLCAdapter::setupEventListeners() {
    if (!m_player) return;
    libvlc_event_manager_t* manager = libvlc_media_player_event_manager(m_player);
    if (!manager) return;
    const auto attach = [manager, this](libvlc_event_e type) {
        libvlc_event_attach(manager, type, &LibVLCAdapter::handleVlcEvent, this);
    };
    attach(libvlc_MediaPlayerPlaying);
    attach(libvlc_MediaPlayerPaused);
    attach(libvlc_MediaPlayerStopped);
    attach(libvlc_MediaPlayerEndReached);
    attach(libvlc_MediaPlayerEncounteredError);
    attach(libvlc_MediaPlayerPositionChanged);
    attach(libvlc_MediaPlayerLengthChanged);
    attach(libvlc_MediaPlayerESAdded);
    attach(libvlc_MediaPlayerESDeleted);
    attach(libvlc_MediaPlayerESSelected);
}

void LibVLCAdapter::handleVlcEvent(const libvlc_event_t* event, void* userData) {
    auto* self = static_cast<LibVLCAdapter*>(userData);
    if (!self || !event) return;
    const auto observers = self->observerSnapshot();
    switch (event->type) {
        case libvlc_MediaPlayerPlaying:
            for (auto* o : observers) if (o) o->onStateChanged(PlaybackState::Playing);
            break;
        case libvlc_MediaPlayerPaused:
            for (auto* o : observers) if (o) o->onStateChanged(PlaybackState::Paused);
            break;
        case libvlc_MediaPlayerStopped:
            for (auto* o : observers) if (o) o->onStateChanged(PlaybackState::Stopped);
            break;
        case libvlc_MediaPlayerEndReached:
            for (auto* o : observers) if (o) o->onStateChanged(PlaybackState::Ended);
            break;
        case libvlc_MediaPlayerEncounteredError:
            for (auto* o : observers) if (o) {
                o->onStateChanged(PlaybackState::Error);
                o->onError("LibVLC reported a playback error.");
            }
            break;
        case libvlc_MediaPlayerPositionChanged: {
            const auto position = self->getPosition();
            for (auto* o : observers) if (o) o->onPositionChanged(position);
            break;
        }
        case libvlc_MediaPlayerLengthChanged:
            for (auto* o : observers) if (o) o->onDurationChanged(event->u.media_player_length_changed.new_length);
            break;
        case libvlc_MediaPlayerESAdded:
        case libvlc_MediaPlayerESDeleted:
        case libvlc_MediaPlayerESSelected:
            for (auto* o : observers) if (o) o->onTracksChanged();
            break;
        default: break;
    }
}

bool LibVLCAdapter::loadMedia(const std::string& uri) {
    if (!m_instance || !m_player || uri.empty()) return false;
    libvlc_media_t* nextMedia = hasUriScheme(uri)
        ? libvlc_media_new_location(m_instance, uri.c_str())
        : libvlc_media_new_path(m_instance, uri.c_str());
    if (!nextMedia) return false;
    libvlc_media_player_set_media(m_player, nextMedia);
    if (m_currentMedia) libvlc_media_release(m_currentMedia);
    m_currentMedia = nextMedia;
    return true;
}
void LibVLCAdapter::play() { if (m_player) libvlc_media_player_play(m_player); }
void LibVLCAdapter::pause() { if (m_player) libvlc_media_player_set_pause(m_player, 1); }
void LibVLCAdapter::stop() { if (m_player) libvlc_media_player_stop(m_player); }

void LibVLCAdapter::seek(float position) {
    if (m_player && std::isfinite(position))
        libvlc_media_player_set_position(m_player, std::clamp(position, 0.0f, 1.0f));
}
void LibVLCAdapter::seekMs(int64_t ms) {
    if (!m_player) return;
    libvlc_time_t target = static_cast<libvlc_time_t>(std::max<int64_t>(0, ms));
    const auto duration = libvlc_media_player_get_length(m_player);
    if (duration > 0) target = std::min(target, duration);
    libvlc_media_player_set_time(m_player, target);
}

PlaybackState LibVLCAdapter::getState() const {
    if (!m_player) return PlaybackState::Stopped;
    switch (libvlc_media_player_get_state(m_player)) {
        case libvlc_Opening: return PlaybackState::Opening;
        case libvlc_Buffering: return PlaybackState::Buffering;
        case libvlc_Playing: return PlaybackState::Playing;
        case libvlc_Paused: return PlaybackState::Paused;
        case libvlc_Stopped: return PlaybackState::Stopped;
        case libvlc_Ended: return PlaybackState::Ended;
        case libvlc_Error: return PlaybackState::Error;
        case libvlc_NothingSpecial:
        default: return PlaybackState::Stopped;
    }
}
PlaybackPosition LibVLCAdapter::getPosition() const {
    PlaybackPosition result;
    if (!m_player) return result;
    result.timeMs = std::max<libvlc_time_t>(0, libvlc_media_player_get_time(m_player));
    result.durationMs = std::max<libvlc_time_t>(0, libvlc_media_player_get_length(m_player));
    const float position = libvlc_media_player_get_position(m_player);
    result.position = std::isfinite(position) ? std::clamp(position, 0.0f, 1.0f) : 0.0f;
    return result;
}

void LibVLCAdapter::setVolume(int volume) {
    if (m_player) libvlc_audio_set_volume(m_player, std::clamp(volume, 0, 200));
}
int LibVLCAdapter::getVolume() const {
    return m_player ? std::clamp(libvlc_audio_get_volume(m_player), 0, 200) : 100;
}
void LibVLCAdapter::setMuted(bool mute) { if (m_player) libvlc_audio_set_mute(m_player, mute ? 1 : 0); }
bool LibVLCAdapter::isMuted() const { return m_player && libvlc_audio_get_mute(m_player) > 0; }
void LibVLCAdapter::setSubtitleDelay(int64_t ms) {
    if (m_player) libvlc_video_set_spu_delay(m_player, millisecondsToMicroseconds(ms));
}
int64_t LibVLCAdapter::getSubtitleDelay() const { return m_player ? libvlc_video_get_spu_delay(m_player) / 1000 : 0; }
void LibVLCAdapter::setAudioDelay(int64_t ms) {
    if (m_player) libvlc_audio_set_delay(m_player, millisecondsToMicroseconds(ms));
}
int64_t LibVLCAdapter::getAudioDelay() const { return m_player ? libvlc_audio_get_delay(m_player) / 1000 : 0; }

bool LibVLCAdapter::addSubtitleFile(const std::string& path) {
    if (!m_player || path.empty()) return false;
    const std::string uri = hasUriScheme(path) ? path : localPathToFileUri(path);
    return libvlc_media_player_add_slave(m_player, libvlc_media_slave_type_subtitle, uri.c_str(), true) == 0;
}

std::vector<SubtitleTrack> LibVLCAdapter::getSubtitleTracks() {
    std::vector<SubtitleTrack> tracks;
    if (!m_player) return tracks;
    auto* descriptions = libvlc_video_get_spu_description(m_player);
    const int selected = libvlc_video_get_spu(m_player);
    for (auto* current = descriptions; current; current = current->p_next) {
        SubtitleTrack track;
        track.id = current->i_id;
        track.name = current->psz_name ? current->psz_name : "";
        track.isSelected = current->i_id == selected;
        tracks.push_back(std::move(track));
    }
    if (descriptions) libvlc_track_description_list_release(descriptions);
    return tracks;
}
void LibVLCAdapter::selectSubtitleTrack(int id) { if (m_player) libvlc_video_set_spu(m_player, id); }

std::vector<AudioTrack> LibVLCAdapter::getAudioTracks() {
    std::vector<AudioTrack> tracks;
    if (!m_player) return tracks;
    auto* descriptions = libvlc_audio_get_track_description(m_player);
    const int selected = libvlc_audio_get_track(m_player);
    for (auto* current = descriptions; current; current = current->p_next) {
        AudioTrack track;
        track.id = current->i_id;
        track.name = current->psz_name ? current->psz_name : "";
        track.isSelected = current->i_id == selected;
        tracks.push_back(std::move(track));
    }
    if (descriptions) libvlc_track_description_list_release(descriptions);
    return tracks;
}
void LibVLCAdapter::selectAudioTrack(int id) { if (m_player) libvlc_audio_set_track(m_player, id); }

void LibVLCAdapter::attachWindow(void* handle) {
    m_windowHandle = handle;
    if (!m_player || !handle) return;
#if defined(__APPLE__)
    libvlc_media_player_set_nsobject(m_player, handle);
#elif defined(_WIN32)
    libvlc_media_player_set_hwnd(m_player, handle);
#else
    libvlc_media_player_set_xwindow(m_player, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handle)));
#endif
}
void LibVLCAdapter::addObserver(IPlayerObserver* observer) {
    if (!observer) return;
    std::lock_guard<std::mutex> lock(m_observerMutex);
    if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) m_observers.push_back(observer);
}
void LibVLCAdapter::removeObserver(IPlayerObserver* observer) {
    std::lock_guard<std::mutex> lock(m_observerMutex);
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}
std::vector<IPlayerObserver*> LibVLCAdapter::observerSnapshot() const {
    std::lock_guard<std::mutex> lock(m_observerMutex);
    return m_observers;
}
} // namespace tomato::engine
