# FindLibVLC.cmake
# Locates LibVLC headers and shared libraries across macOS, Linux, and Windows.

find_path(LIBVLC_INCLUDE_DIR
    NAMES vlc/vlc.h
    PATHS
        /Applications/VLC.app/Contents/MacOS/include
        /usr/include
        /usr/local/include
        /opt/homebrew/include
        ENV LIBVLC_INCLUDE_DIR
)

find_library(LIBVLC_LIBRARY
    NAMES vlc libvlc
    PATHS
        /Applications/VLC.app/Contents/MacOS/lib
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
        ENV LIBVLC_LIBRARY
)

find_library(LIBVLCCORE_LIBRARY
    NAMES vlccore libvlccore
    PATHS
        /Applications/VLC.app/Contents/MacOS/lib
        /usr/lib
        /usr/local/lib
        /opt/homebrew/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibVLC
    DEFAULT_MSG
    LIBVLC_LIBRARY
    LIBVLC_INCLUDE_DIR
)

if(LibVLC_FOUND)
    set(LIBVLC_LIBRARIES ${LIBVLC_LIBRARY})
    if(LIBVLCCORE_LIBRARY)
        list(APPEND LIBVLC_LIBRARIES ${LIBVLCCORE_LIBRARY})
    endif()
    set(LIBVLC_INCLUDE_DIRS ${LIBVLC_INCLUDE_DIR})

    if(NOT TARGET LibVLC::LibVLC)
        add_library(LibVLC::LibVLC UNKNOWN IMPORTED)
        set_target_properties(LibVLC::LibVLC PROPERTIES
            IMPORTED_LOCATION "${LIBVLC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBVLC_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(LIBVLC_INCLUDE_DIR LIBVLC_LIBRARY LIBVLCCORE_LIBRARY)
