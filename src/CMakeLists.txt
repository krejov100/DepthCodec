cmake_minimum_required (VERSION 3.0.0)

option (USE_SUPERBUILD "Whether or not a superbuild should be invoked" ON)

set(CMAKE_CXX_STANDARD 17)

MESSAGE(STATUS ${CMAKE_CXX_COMPILER_ID})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.3")
        message(FATAL_ERROR "Insufficient gcc version")
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "your.required.msvc.version")
        message(FATAL_ERROR "Insufficient msvc version")
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

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()
message(STATUS test)
add_subdirectory (modules)







