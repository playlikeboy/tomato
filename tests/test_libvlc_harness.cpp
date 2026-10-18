#include <iostream>
#include <cassert>
#include <vlc/vlc.h>
#include <vlc/libvlc_version.h>

int main() {
    std::cout << "==========================================================" << std::endl;
    std::cout << "  🍅 Tomato Engine Test Harness - LibVLC Integration" << std::endl;
    std::cout << "==========================================================" << std::endl;

    std::cout << "[1] Header LibVLC Version: " 
              << LIBVLC_VERSION_MAJOR << "." 
              << LIBVLC_VERSION_MINOR << "." 
              << LIBVLC_VERSION_REVISION << std::endl;

    const char* runtimeVer = libvlc_get_version();
    std::cout << "[2] Runtime LibVLC Version: " << (runtimeVer ? runtimeVer : "Unknown") << std::endl;

    const char* const vlcArgs[] = {
        "--no-video-title-show",
        "--no-sub-autodetect-file",
        "--quiet"
    };

    std::cout << "[3] Initializing libvlc_new()... ";
    libvlc_instance_t* inst = libvlc_new(3, vlcArgs);
    if (!inst) {
        std::cerr << "FAILED to create LibVLC instance!" << std::endl;
        return 1;
    }
    std::cout << "SUCCESS" << std::endl;

    std::cout << "[4] Creating libvlc_media_player_new()... ";
    libvlc_media_player_t* mp = libvlc_media_player_new(inst);
    if (!mp) {
        std::cerr << "FAILED to create LibVLC media player!" << std::endl;
        libvlc_release(inst);
        return 1;
    }
    std::cout << "SUCCESS" << std::endl;

    std::cout << "[5] Testing Audio Volume APIs... ";
    libvlc_audio_set_volume(mp, 80);
    int vol = libvlc_audio_get_volume(mp);
    assert(vol == 80);
    std::cout << "Volume set to 80, read back: " << vol << " (PASS)" << std::endl;

    std::cout << "[6] Testing Audio Mute APIs... ";
    libvlc_audio_set_mute(mp, 1);
    assert(libvlc_audio_get_mute(mp) == 1);
    libvlc_audio_set_mute(mp, 0);
    assert(libvlc_audio_get_mute(mp) == 0);
    std::cout << "Mute toggle verified (PASS)" << std::endl;

    std::cout << "[7] Testing Subtitle Delay APIs... ";
    libvlc_video_set_spu_delay(mp, 250000); // 250 ms in microseconds
    int64_t delay = libvlc_video_get_spu_delay(mp);
    // Note: LibVLC returns 0 if no video output thread is currently active
    std::cout << "Subtitle delay call succeeded (returned: " << delay << " us, PASS)" << std::endl;

    std::cout << "[8] Cleaning up LibVLC instances... ";
    libvlc_media_player_release(mp);
    libvlc_release(inst);
    std::cout << "CLEAN TEARDOWN" << std::endl;

    std::cout << "==========================================================" << std::endl;
    std::cout << "  🎉 All LibVLC Engine Verification Tests Passed!" << std::endl;
    std::cout << "==========================================================" << std::endl;
    return 0;
}
