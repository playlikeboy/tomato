#include <vlc/libvlc_version.h>
#include <vlc/vlc.h>

#include <iostream>

int main() {
    std::cout << "[Tomato] LibVLC lifecycle smoke test\n";
    const char* version = libvlc_get_version();
    if (!version) {
        std::cerr << "FAIL: LibVLC did not report a runtime version.\n";
        return 1;
    }
    std::cout << "Runtime LibVLC: " << version << '\n';

    const char* const args[] = {
        "--no-video-title-show",
        "--no-snapshot-preview",
        "--quiet"
    };
    libvlc_instance_t* instance = libvlc_new(3, args);
    if (!instance) {
        std::cerr << "FAIL: libvlc_new() returned null. Check LibVLC plugins/runtime.\n";
        return 1;
    }

    libvlc_media_player_t* player = libvlc_media_player_new(instance);
    if (!player) {
        std::cerr << "FAIL: libvlc_media_player_new() returned null.\n";
        libvlc_release(instance);
        return 1;
    }

    // Track and audio-output APIs often depend on loaded media/output. This
    // harness intentionally tests only the no-media lifecycle, not playback.
    libvlc_media_player_release(player);
    libvlc_release(instance);
    std::cout << "PASS: LibVLC instance and player created and released cleanly.\n";
    return 0;
}
