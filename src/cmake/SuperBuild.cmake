
include (ExternalProject)

set_property (DIRECTORY PROPERTY EP_BASE Dependencies)

set (DEPENDENCIES)
set (EXTRA_CMAKE_ARGS)

####################
###############BOOST
####################
# Use static linking to avoid issues with system-wide installations of Boost.
list (APPEND DEPENDENCIES ep_boost)
ExternalProject_Add (ep_boost
        URL http://sourceforge.net/projects/boost/files/boost/1.67.0/boost_1_67_0.tar.gz
        URL_MD5 4850fceb3f2222ee011d4f3ea304d2cb
        CONFIGURE_COMMAND ./bootstrap.sh
        BUILD_COMMAND ./b2 link=static
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND ""
        )
set(BOOST_ROOT ${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/ep_boost CACHE PATH boost root dir)
list (APPEND EXTRA_CMAKE_ARGS
        -DBOOST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Source/ep_boost
        -DBoost_NO_SYSTEM_PATHS=ON)

####################
##############OPENCV
####################

list (APPEND DEPENDENCIES ep_opencv)
set(OpenCV_TAG "3.4.1")
ExternalProject_Add(ep_opencv
        GIT_REPOSITORY "http://github.com/Itseez/opencv.git"
        GIT_TAG "${OpenCV_TAG}"
        SOURCE_DIR opencv
        BINARY_DIR opencv-build
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
        ${ep_common_args}
        -DBUILD_DOCS:BOOL=FALSE
        -DBUILD_EXAMPLES:BOOL=FALSE
        -DBUILD_TESTS:BOOL=FALSE
        -DBUILD_SHARED_LIBS:BOOL=TRUE
        -DWITH_CUDA:BOOL=FALSE
        -DWITH_FFMPEG:BOOL=FALSE
        -DBUILD_PERF_TESTS:BOOL=FALSE
        INSTALL_COMMAND ""
        )

set(OpenCV_DIR ${CMAKE_BINARY_DIR}/opencv-build CACHE PATH opencv root dir)

list (APPEND EXTRA_CMAKE_ARGS
        -DOpenCV_DIR=${CMAKE_BINARY_DIR}/opencv-build)

# FIXME add to default target "all"?
ExternalProject_Add (ep_depthcodec
        DEPENDS ${DEPENDENCIES}
        SOURCE_DIR ${PROJECT_SOURCE_DIR}
        CMAKE_ARGS -DUSE_SUPERBUILD=OFF ${EXTRA_CMAKE_ARGS}
        INSTALL_COMMAND ""
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/depthcodec)