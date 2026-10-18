#pragma once

#include "engine/player-interface/player_interface.h"
#include <mutex>
#include <vector>

// Forward declarations of LibVLC C structs
struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_media_t;
struct libvlc_event_t;

namespace tomato::engine {

class LibVLCAdapter final : public IPlayerEngine {
public:
    LibVLCAdapter();
    ~LibVLCAdapter() override;

    bool initialize(const std::vector<std::string>& customArgs = {});
    void cleanup();

    // IPlayerEngine implementation
    bool loadMedia(const std::string& uri) override;
    void play() override;
    void pause() override;
    void stop() override;

    void seek(float position0to1) override;
    void seekMs(int64_t timeMs) override;
    PlaybackState getState() const override;
    PlaybackPosition getPosition() const override;

    void setVolume(int volume) override;
    int getVolume() const override;
    void setMuted(bool mute) override;
    bool isMuted() const override;

    void setSubtitleDelay(int64_t delayMs) override;
    int64_t getSubtitleDelay() const override;
    void setAudioDelay(int64_t delayMs) override;
    int64_t getAudioDelay() const override;
    bool addSubtitleFile(const std::string& path) override;

    std::vector<SubtitleTrack> getSubtitleTracks() override;
    void selectSubtitleTrack(int trackId) override;
    std::vector<AudioTrack> getAudioTracks() override;
    void selectAudioTrack(int trackId) override;

    void attachWindow(void* nativeWindowHandle) override;

    void addObserver(IPlayerObserver* observer) override;
    void removeObserver(IPlayerObserver* observer) override;

private:
    static void handleVlcEvent(const libvlc_event_t* p_event, void* p_data);
    void setupEventListeners();
    void notifyStateChanged(PlaybackState state);

    libvlc_instance_t* m_instance = nullptr;
    libvlc_media_player_t* m_player = nullptr;
    libvlc_media_t* m_currentMedia = nullptr;

    mutable std::mutex m_observerMutex;
    std::vector<IPlayerObserver*> m_observers;
    void* m_windowHandle = nullptr;
};

} // namespace tomato::engine
