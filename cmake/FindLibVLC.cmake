# FindLibVLC.cmake - locate LibVLC SDK headers and library.

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(PC_LIBVLC QUIET libvlc)
endif()

set(_LIBVLC_INCLUDE_HINTS ${PC_LIBVLC_INCLUDE_DIRS})
set(_LIBVLC_LIBRARY_HINTS ${PC_LIBVLC_LIBRARY_DIRS})

if(DEFINED LIBVLC_SDK_PATH AND NOT LIBVLC_SDK_PATH STREQUAL "")
    list(APPEND _LIBVLC_INCLUDE_HINTS "${LIBVLC_SDK_PATH}/include" "${LIBVLC_SDK_PATH}/sdk/include")
    list(APPEND _LIBVLC_LIBRARY_HINTS "${LIBVLC_SDK_PATH}/lib" "${LIBVLC_SDK_PATH}/sdk/lib" "${LIBVLC_SDK_PATH}/lib/x64")
endif()
if(DEFINED ENV{VLC_SDK_PATH} AND NOT "$ENV{VLC_SDK_PATH}" STREQUAL "")
    list(APPEND _LIBVLC_INCLUDE_HINTS "$ENV{VLC_SDK_PATH}/include" "$ENV{VLC_SDK_PATH}/sdk/include")
    list(APPEND _LIBVLC_LIBRARY_HINTS "$ENV{VLC_SDK_PATH}/lib" "$ENV{VLC_SDK_PATH}/sdk/lib" "$ENV{VLC_SDK_PATH}/lib/x64")
endif()

find_path(LIBVLC_INCLUDE_DIR NAMES vlc/vlc.h
    HINTS ${_LIBVLC_INCLUDE_HINTS}
    PATHS /Applications/VLC.app/Contents/MacOS/include /usr/include /usr/local/include /opt/homebrew/include
    ENV LIBVLC_INCLUDE_DIR
)
find_library(LIBVLC_LIBRARY NAMES vlc libvlc
    HINTS ${_LIBVLC_LIBRARY_HINTS}
    PATHS /Applications/VLC.app/Contents/MacOS/lib /usr/lib /usr/local/lib /opt/homebrew/lib
    ENV LIBVLC_LIBRARY
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibVLC REQUIRED_VARS LIBVLC_LIBRARY LIBVLC_INCLUDE_DIR)

if(LibVLC_FOUND)
    set(LIBVLC_INCLUDE_DIRS "${LIBVLC_INCLUDE_DIR}")
    set(LIBVLC_LIBRARIES "${LIBVLC_LIBRARY}")
    if(NOT TARGET LibVLC::LibVLC)
        add_library(LibVLC::LibVLC UNKNOWN IMPORTED)
        set_target_properties(LibVLC::LibVLC PROPERTIES
            IMPORTED_LOCATION "${LIBVLC_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LIBVLC_INCLUDE_DIR}"
        )
    endif()
endif()
mark_as_advanced(LIBVLC_INCLUDE_DIR LIBVLC_LIBRARY)
