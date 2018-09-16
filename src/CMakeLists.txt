cmake_minimum_required (VERSION 3.0.0)

project(DepthCodec VERSION 1.0.0 DESCRIPTION "Compression of Depth")

option (USE_SUPERBUILD "Whether or not a superbuild should be invoked" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
cmake_policy(SET CMP0074 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

MESSAGE(STATUS ${CMAKE_CXX_COMPILER_ID})
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.3")
        message(FATAL_ERROR "Insufficient gcc version")
    endif()
endif()

MESSAGE(STATUS "USE_SUPERBUILD = ${USE_SUPERBUILD}")

if (USE_SUPERBUILD)
    project (SUPERBUILD NONE)
    # execute the superbuild (this script will be invoked again without the
    # USE_SUPERBUILD option this time)
    include (cmake/SuperBuild.cmake)
    return()                      # stop processing this file further
else()
    #if(USE_IDE)
    #    MESSAGE(STATUS "OpenCV Headers ${OpenCV_INCLUDE_DIRS}")
    #    MESSAGE(STATUS "OpenCV Libs ${OpenCV_LIBRARY_DIR}")
			
    #endif()
    project(DepthCodec)
endif()

#move to superbuild
#if (UNIX)
#	set(realsense2_DIR "/usr/local/lib/cmake/realsense2/realsense2Config.cmake" CACHE PATH "librealsense installation directory")
#elseif(WIN32)
#	set(realsense2_DIR "C:/Program Files (x86)/Intel RealSense SDK 2.0" CACHE PATH "librealsense installation directory")
#endif()

# Simple non robust way to find the librealsense library
if(WIN32)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x64")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x64")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
    else()
        set(LIBRARY_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\lib\\x86")          # TODO: Update this variable to correct path - folder where realsense2.lib is found
        set(DLL_DIR "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\bin\\x86")              # TODO: Update this variable to correct path - folder where realsense2.dll is found
    endif()
    set(PROJECT_BINARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/\$\(Configuration\)")                # TODO: Update this variable to correct path - folder to which your project will compile
    set(librealsense_INCLUDE_DIRS "C:\\Program Files (x86)\\Intel RealSense SDK 2.0\\include")   # TODO: Update this variable to correct path - folder where librealsense2 folder is found
endif()
find_library(REALSENSE2_FOUND realsense2 HINTS ${LIBRARY_DIR} REQUIRED)
if(NOT REALSENSE2_FOUND)
    SET(REALSENSE2_FOUND "realsense2")
    message(WARN "Failed to find_library(realsense2)")
endif()

#requires Open3D
#set(Boost_USE_STATIC_LIBS ON)
#message(STATUS BOOST_ROOT ${BOOST_ROOT})
#find_package (Boost COMPONENTS system program_options log serialization REQUIRED)
#ADD_DEFINITIONS(-BOOST_ALL_DYN_LINK)

#requires Open3D
#MESSAGE(STATUS "Looking in for Open3D ${INSTALL_DEPENDENCIES_DIR}/Open3D/CMake")
#find_package(Open3D REQUIRED HINTS ${INSTALL_DEPENDENCIES_DIR}/Open3D/CMake)
#MESSAGE(STATUS Open3D Headers"${Open3D_INCLUDE_DIRS}")


#pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()
add_subdirectory (modules)







