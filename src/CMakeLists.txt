cmake_minimum_required (VERSION 3.0.0)

option (USE_SUPERBUILD "Whether or not a superbuild should be invoked" ON)

set(CMAKE_CXX_STANDARD 17)

MESSAGE(STATUS ${CMAKE_CXX_COMPILER_ID})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.3")
        message(FATAL_ERROR "Insufficient gcc version")
    endif()
endif()

if (USE_SUPERBUILD)
    project (SUPERBUILD NONE)
    # execute the superbuild (this script will be invoked again without the
    # USE_SUPERBUILD option this time)
    include (cmake/SuperBuild.cmake)
    return()                      # stop processing this file further
else()
    if(USE_IDE)
        set(BOOST_ROOT ${CMAKE_BINARY_DIR}/Dependencies/Source/ep_boost)
        set(OpenCV_DIR ${CMAKE_BINARY_DIR}/Dependencies/Source/opencv-build)
    endif()
    project(DepthCodec)
endif()

#move to superbuild
set(realsense2_DIR "/usr/local/lib/cmake/realsense2/realsense2Config.cmake" CACHE PATH "librealsense installation directory")
find_package(realsense2 REQUIRED HINTS ${realsense2_DIR} ${CMAKE_SOURCE_DIR}/3rdParty/librealsense-master/build/realsense2Config.cmake)

#requires Open3D
set(Open3D_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/Open3D-master/build/CMakeFiles")
find_package(Open3D REQUIRED HINTS ${Open3D_DIR})
LINK_DIRECTORIES(${Open3D_LIBRARY_DIRS})
MESSAGE(STATUS Open3D Headers"${Open3D_INCLUDE_DIRS}")


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()
add_subdirectory (modules)







