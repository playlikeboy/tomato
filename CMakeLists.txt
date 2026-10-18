cmake_minimum_required(VERSION 3.22)

project(Tomato
    VERSION 0.1.0
    DESCRIPTION "A modern, beautifully designed, open-source desktop media player"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Module path for custom finders
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 1. Discover LibVLC
find_package(LibVLC)

# 2. Tomato Core Library
add_library(tomato_core STATIC
    core/playback/queue_manager.cpp
)
target_include_directories(tomato_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 3. Tomato Engine Library (if LibVLC found)
if(LibVLC_FOUND)
    message(STATUS "LibVLC Found: ${LIBVLC_LIBRARY}")
    message(STATUS "LibVLC Include: ${LIBVLC_INCLUDE_DIR}")

    add_library(tomato_engine STATIC
        engine/libvlc-adapter/libvlc_adapter.cpp
    )
    target_link_libraries(tomato_engine PUBLIC
        tomato_core
        LibVLC::LibVLC
    )
    target_include_directories(tomato_engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBVLC_INCLUDE_DIRS}
    )

    # LibVLC native test harness
    add_executable(test_libvlc_harness
        tests/test_libvlc_harness.cpp
    )
    target_link_libraries(test_libvlc_harness PRIVATE
        LibVLC::LibVLC
    )
    target_include_directories(test_libvlc_harness PRIVATE
        ${LIBVLC_INCLUDE_DIRS}
    )
    if(APPLE)
        set_target_properties(test_libvlc_harness PROPERTIES
            BUILD_RPATH "/Applications/VLC.app/Contents/MacOS/lib"
            INSTALL_RPATH "/Applications/VLC.app/Contents/MacOS/lib"
        )
    endif()
else()
    message(WARNING "LibVLC not found. Building core logic without engine adapter.")
endif()

# 4. Core Unit Tests
add_executable(test_queue_manager
    tests/test_queue_manager.cpp
)
target_link_libraries(test_queue_manager PRIVATE
    tomato_core
)

# Enable automated testing with CTest
enable_testing()
add_test(NAME QueueManagerTest COMMAND test_queue_manager)
if(LibVLC_FOUND)
    add_test(NAME LibVLCHarnessTest COMMAND test_libvlc_harness)
endif()
