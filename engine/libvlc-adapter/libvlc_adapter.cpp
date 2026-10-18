#include "engine/libvlc-adapter/libvlc_adapter.h"
#include <vlc/vlc.h>
#include <algorithm>
#include <iostream>

namespace tomato::engine {

LibVLCAdapter::LibVLCAdapter() {
    initialize();
}

LibVLCAdapter::~LibVLCAdapter() {
    cleanup();
}

bool LibVLCAdapter::initialize(const std::vector<std::string>& customArgs) {
    if (m_instance) {
        return true;
    }

    std::vector<const char*> defaultArgs = {
        "--no-video-title-show",
        "--no-sub-autodetect-file",
        "--no-snapshot-preview",
        "--quiet"
    };

    std::vector<const char*> allArgs = defaultArgs;
    for (const auto& arg : customArgs) {
        allArgs.push_back(arg.c_str());
    }

    m_instance = libvlc_new(static_cast<int>(allArgs.size()), allArgs.data());
    if (!m_instance) {
        std::cerr << "[Tomato::LibVLCAdapter] Failed to initialize LibVLC instance" << std::endl;
        return false;
    }

    m_player = libvlc_media_player_new(m_instance);
    if (!m_player) {
        std::cerr << "[Tomato::LibVLCAdapter] Failed to create media player" << std::endl;
        libvlc_release(m_instance);
        m_instance = nullptr;
        return false;
    }

    setupEventListeners();
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
    libvlc_event_manager_t* em = libvlc_media_player_event_manager(m_player);
    if (!em) return;

    libvlc_event_attach(em, libvlc_MediaPlayerPlaying, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPaused, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerStopped, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerEncounteredError, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPositionChanged, &LibVLCAdapter::handleVlcEvent, this);
    libvlc_event_attach(em, libvlc_MediaPlayerLengthChanged, &LibVLCAdapter::handleVlcEvent, this);
}

void LibVLCAdapter::handleVlcEvent(const libvlc_event_t* p_event, void* p_data) {
    auto* self = static_cast<LibVLCAdapter*>(p_data);
    if (!self || !p_event) return;

    std::lock_guard<std::mutex> lock(self->m_observerMutex);
    switch (p_event->type) {
        case libvlc_MediaPlayerPlaying:
            for (auto* obs : self->m_observers) obs->onStateChanged(PlaybackState::Playing);
            break;
        case libvlc_MediaPlayerPaused:
            for (auto* obs : self->m_observers) obs->onStateChanged(PlaybackState::Paused);
            break;
        case libvlc_MediaPlayerStopped:
            for (auto* obs : self->m_observers) obs->onStateChanged(PlaybackState::Stopped);
            break;
        case libvlc_MediaPlayerEndReached:
            for (auto* obs : self->m_observers) obs->onStateChanged(PlaybackState::Ended);
            break;
        case libvlc_MediaPlayerEncounteredError:
            for (auto* obs : self->m_observers) {
                obs->onStateChanged(PlaybackState::Error);
                obs->onError("Playback error encountered in LibVLC");
            }
            break;
        case libvlc_MediaPlayerPositionChanged: {
            PlaybackPosition pos = self->getPosition();
            for (auto* obs : self->m_observers) obs->onPositionChanged(pos);
            break;
        }
        case libvlc_MediaPlayerLengthChanged:
            for (auto* obs : self->m_observers) {
                obs->onDurationChanged(p_event->u.media_player_length_changed.new_length);
            }
            break;
        default:
            break;
    }
}

bool LibVLCAdapter::loadMedia(const std::string& uri) {
    if (!m_instance || !m_player) return false;

    if (m_currentMedia) {
        libvlc_media_release(m_currentMedia);
        m_currentMedia = nullptr;
    }

    if (uri.find("://") != std::string::npos) {
        m_currentMedia = libvlc_media_new_location(m_instance, uri.c_str());
    } else {
        m_currentMedia = libvlc_media_new_path(m_instance, uri.c_str());
    }

    if (!m_currentMedia) {
        return false;
    }

    libvlc_media_player_set_media(m_player, m_currentMedia);
    return true;
}

void LibVLCAdapter::play() {
    if (m_player) {
        libvlc_media_player_play(m_player);
    }
}

void LibVLCAdapter::pause() {
    if (m_player) {
        libvlc_media_player_pause(m_player);
    }
}

void LibVLCAdapter::stop() {
    if (m_player) {
        libvlc_media_player_stop(m_player);
    }
}

void LibVLCAdapter::seek(float position0to1) {
    if (m_player) {
        position0to1 = std::clamp(position0to1, 0.0f, 1.0f);
        libvlc_media_player_set_position(m_player, position0to1);
    }
}

void LibVLCAdapter::seekMs(int64_t timeMs) {
    if (m_player) {
        libvlc_media_player_set_time(m_player, static_cast<libvlc_time_t>(timeMs));
    }
}

PlaybackState LibVLCAdapter::getState() const {
    if (!m_player) return PlaybackState::Stopped;
    libvlc_state_t state = libvlc_media_player_get_state(m_player);
    switch (state) {
        case libvlc_Opening:   return PlaybackState::Opening;
        case libvlc_Buffering: return PlaybackState::Buffering;
        case libvlc_Playing:   return PlaybackState::Playing;
        case libvlc_Paused:    return PlaybackState::Paused;
        case libvlc_Stopped:   return PlaybackState::Stopped;
        case libvlc_Ended:     return PlaybackState::Ended;
        case libvlc_Error:     return PlaybackState::Error;
        default:               return PlaybackState::Stopped;
    }
}

PlaybackPosition LibVLCAdapter::getPosition() const {
    PlaybackPosition pos;
    if (m_player) {
        pos.timeMs = libvlc_media_player_get_time(m_player);
        pos.durationMs = libvlc_media_player_get_length(m_player);
        pos.position = libvlc_media_player_get_position(m_player);
    }
    return pos;
}

void LibVLCAdapter::setVolume(int volume) {
    if (m_player) {
        volume = std::clamp(volume, 0, 200);
        libvlc_audio_set_volume(m_player, volume);
    }
}

int LibVLCAdapter::getVolume() const {
    if (m_player) {
        return libvlc_audio_get_volume(m_player);
    }
    return 100;
}

void LibVLCAdapter::setMuted(bool mute) {
    if (m_player) {
        libvlc_audio_set_mute(m_player, mute ? 1 : 0);
    }
}

bool LibVLCAdapter::isMuted() const {
    if (m_player) {
        return libvlc_audio_get_mute(m_player) > 0;
    }
    return false;
}

void LibVLCAdapter::setSubtitleDelay(int64_t delayMs) {
    if (m_player) {
        libvlc_video_set_spu_delay(m_player, delayMs * 1000); // LibVLC uses microseconds
    }
}

int64_t LibVLCAdapter::getSubtitleDelay() const {
    if (m_player) {
        return libvlc_video_get_spu_delay(m_player) / 1000;
    }
    return 0;
}

void LibVLCAdapter::setAudioDelay(int64_t delayMs) {
    if (m_player) {
        libvlc_audio_set_delay(m_player, delayMs * 1000);
    }
}

int64_t LibVLCAdapter::getAudioDelay() const {
    if (m_player) {
        return libvlc_audio_get_delay(m_player) / 1000;
    }
    return 0;
}

bool LibVLCAdapter::addSubtitleFile(const std::string& path) {
    if (m_player) {
        std::string uri = path;
        if (uri.find("://") == std::string::npos) {
            uri = "file://" + uri;
        }
        return libvlc_media_player_add_slave(m_player, libvlc_media_slave_type_subtitle, uri.c_str(), true) == 0;
    }
    return false;
}

std::vector<SubtitleTrack> LibVLCAdapter::getSubtitleTracks() {
    std::vector<SubtitleTrack> tracks;
    if (!m_player) return tracks;

    libvlc_track_description_t* desc = libvlc_video_get_spu_description(m_player);
    int currentTrack = libvlc_video_get_spu(m_player);

    for (libvlc_track_description_t* cur = desc; cur != nullptr; cur = cur->p_next) {
        SubtitleTrack t;
        t.id = cur->i_id;
        t.name = cur->psz_name ? cur->psz_name : "";
        t.isSelected = (cur->i_id == currentTrack);
        tracks.push_back(t);
    }
    libvlc_track_description_list_release(desc);
    return tracks;
}

void LibVLCAdapter::selectSubtitleTrack(int trackId) {
    if (m_player) {
        libvlc_video_set_spu(m_player, trackId);
    }
}

std::vector<AudioTrack> LibVLCAdapter::getAudioTracks() {
    std::vector<AudioTrack> tracks;
    if (!m_player) return tracks;

    libvlc_track_description_t* desc = libvlc_audio_get_track_description(m_player);
    int currentTrack = libvlc_audio_get_track(m_player);

    for (libvlc_track_description_t* cur = desc; cur != nullptr; cur = cur->p_next) {
        AudioTrack t;
        t.id = cur->i_id;
        t.name = cur->psz_name ? cur->psz_name : "";
        t.isSelected = (cur->i_id == currentTrack);
        tracks.push_back(t);
    }
    libvlc_track_description_list_release(desc);
    return tracks;
}

void LibVLCAdapter::selectAudioTrack(int trackId) {
    if (m_player) {
        libvlc_audio_set_track(m_player, trackId);
    }
}

void LibVLCAdapter::attachWindow(void* nativeWindowHandle) {
    m_windowHandle = nativeWindowHandle;
    if (!m_player || !nativeWindowHandle) return;

#if defined(__APPLE__)
    libvlc_media_player_set_nsobject(m_player, nativeWindowHandle);
#elif defined(_WIN32)
    libvlc_media_player_set_hwnd(m_player, nativeWindowHandle);
#else
    libvlc_media_player_set_xwindow(m_player, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(nativeWindowHandle)));
#endif
}

void LibVLCAdapter::addObserver(IPlayerObserver* observer) {
    std::lock_guard<std::mutex> lock(m_observerMutex);
    if (observer && std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
        m_observers.push_back(observer);
    }
}

void LibVLCAdapter::removeObserver(IPlayerObserver* observer) {
    std::lock_guard<std::mutex> lock(m_observerMutex);
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

} // namespace tomato::engine
