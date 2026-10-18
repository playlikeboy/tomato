#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace tomato::engine {

enum class PlaybackState {
    Stopped,
    Opening,
    Buffering,
    Playing,
    Paused,
    Ended,
    Error
};

struct MediaTrack {
    int id = -1;
    std::string name;
    std::string language;
    bool isSelected = false;
};

using SubtitleTrack = MediaTrack;
using AudioTrack = MediaTrack;

struct PlaybackPosition {
    int64_t timeMs = 0;
    int64_t durationMs = 0;
    float position = 0.0f; // 0.0 to 1.0
};

class IPlayerObserver {
public:
    virtual ~IPlayerObserver() = default;
    virtual void onStateChanged(PlaybackState state) = 0;
    virtual void onPositionChanged(const PlaybackPosition& pos) = 0;
    virtual void onDurationChanged(int64_t durationMs) = 0;
    virtual void onTracksChanged() = 0;
    virtual void onError(const std::string& message) = 0;
};

class IPlayerEngine {
public:
    virtual ~IPlayerEngine() = default;

    // Media Lifecycle
    virtual bool loadMedia(const std::string& uri) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    // Navigation & Seeking
    virtual void seek(float position0to1) = 0;
    virtual void seekMs(int64_t timeMs) = 0;
    virtual PlaybackState getState() const = 0;
    virtual PlaybackPosition getPosition() const = 0;

    // Audio & Volume
    virtual void setVolume(int volume) = 0; // 0 - 100 (supports > 100 software amplification)
    virtual int getVolume() const = 0;
    virtual void setMuted(bool mute) = 0;
    virtual bool isMuted() const = 0;

    // Subtitles & Track Management
    virtual void setSubtitleDelay(int64_t delayMs) = 0;
    virtual int64_t getSubtitleDelay() const = 0;
    virtual void setAudioDelay(int64_t delayMs) = 0;
    virtual int64_t getAudioDelay() const = 0;
    virtual bool addSubtitleFile(const std::string& path) = 0;

    virtual std::vector<SubtitleTrack> getSubtitleTracks() = 0;
    virtual void selectSubtitleTrack(int trackId) = 0;
    virtual std::vector<AudioTrack> getAudioTracks() = 0;
    virtual void selectAudioTrack(int trackId) = 0;

    // Native Platform Window Embedding
    virtual void attachWindow(void* nativeWindowHandle) = 0;

    // Observer Pattern
    virtual void addObserver(IPlayerObserver* observer) = 0;
    virtual void removeObserver(IPlayerObserver* observer) = 0;
};

} // namespace tomato::engine
