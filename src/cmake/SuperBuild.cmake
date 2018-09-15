
cmake_minimum_required (VERSION 3.0.0)

project( DepthCodec-Superbuild )

# set CMAKE_MODULE_PATH for cmake macro/function and modules
set( CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/../cmake
)

option( BUILD_SHARED_LIBS "Build shared libraries" ON )

include (ExternalProject)

set( BUILD_TYPE ${CMAKE_BUILD_TYPE} )
if( NOT INSTALL_DEPENDENCIES_DIR )
  set( INSTALL_DEPENDENCIES_DIR ${CMAKE_BINARY_DIR}/INSTALL CACHE STRING "Install directory for dependencies")
endif()
set( Patches_DIR ${CMAKE_CURRENT_SOURCE_DIR}/patches )
set( DepthCodec_DEPENDENCIES )

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

####################
###############Depnds
####################
option( USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

	set(git_protocol "git")
	if(NOT USE_GIT_PROTOCOL)
        set(git_protocol "http")
	endif()

option( USE_SYSTEM_Boost "Use system libraries for Boost" OFF )
if( ${USE_SYSTEM_Boost} MATCHES "OFF" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Boost.cmake)
  set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} Boost )
else()
  find_package( Boost REQUIRED )
endif()

option( USE_SYSTEM_OpenCV "Use system libraries for OpenCV" OFF )
if( ${USE_SYSTEM_OpenCV} MATCHES "OFF" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-OpenCV.cmake)
  set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} OpenCV )
else()
  find_package( OpenCV REQUIRED )
endif()

option( USE_SYSTEM_Open3D "Use system libraries for Open3D" OFF )
if( ${USE_SYSTEM_Open3D} MATCHES "OFF" )
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Open3D.cmake)
  set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} Open3D )
else()
  find_package( Open3D REQUIRED )
endif()

ExternalProject_Add (DepthCodec
        DEPENDS ${DepthCodec_DEPENDENCIES}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/DepthCodec
        CMAKE_GENERATOR ${EP_CMAKE_GENERATOR}
        CMAKE_ARGS
        ${ep_common_args}
        -DUSE_SUPERBUILD=OFF
        #-DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE}
        #-DBoost_INCLUDE_DIR:PATH=${Boost_INCLUDE_DIR}
        #-DBOOST_ROOT:PATH=${BOOST_ROOT}
        #-DINSTALL_DEPENDENCIES_DIR:PATH=${INSTALL_DEPENDENCIES_DIR}
        INSTALL_COMMAND ""
        )

####################
##############OPENCV
####################

#list (APPEND DEPENDENCIES ep_opencv)
#set(OpenCV_TAG "3.4.1")
#ExternalProject_Add(ep_opencv
#        GIT_REPOSITORY "http://github.com/Itseez/opencv.git"
#        GIT_TAG "${OpenCV_TAG}"
#        SOURCE_DIR opencv
#        BINARY_DIR opencv-build
#        CMAKE_GENERATOR ${gen}
#        CMAKE_ARGS
#        ${ep_common_args}
#        -DBUILD_DOCS:BOOL=FALSE
#        -DBUILD_EXAMPLES:BOOL=FALSE
#        -DBUILD_TESTS:BOOL=FALSE
#        -DBUILD_SHARED_LIBS:BOOL=TRUE
#        -DWITH_CUDA:BOOL=FALSE
#        -DWITH_FFMPEG:BOOL=FALSE
#        -DBUILD_PERF_TESTS:BOOL=FALSE
#        INSTALL_COMMAND ""
#        )

#set(OpenCV_DIR ${CMAKE_BINARY_DIR}/opencv-build CACHE PATH opencv root dir)

#list (APPEND EXTRA_CMAKE_ARGS
 #       -DOpenCV_DIR=${CMAKE_BINARY_DIR}/opencv-build)
