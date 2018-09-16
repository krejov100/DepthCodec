message(STATUS "External project: OpenCV")

ExternalProject_Add(OpenCV
		GIT_REPOSITORY ${git_protocol}://github.com/cedricve/opencv
        SOURCE_DIR opencv
        BINARY_DIR opencv-build
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS
        ${ep_common_args}
        -DBUILD_DOCS:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
		-DBUILD_opencv_python2:BOOL=OFF
		-DBUILD_opencv_python3:BOOL=OFF
        -DBUILD_WITH_DEBUG_INFO=OFF
        -DBUILD_PACKAGE:BOOL=OFF
        -DBUILD_opencv_core=ON
        -DBUILD_opencv_imgproc=OFF #
        -DBUILD_opencv_highgui=OFF #
        -DBUILD_opencv_video=OFF
        -DBUILD_opencv_apps=OFF
        -DBUILD_opencv_flann=OFF
        -DBUILD_opencv_gpu=OFF
        -DBUILD_opencv_ml=OFF
        -DBUILD_opencv_legacy=OFF
        -DBUILD_opencv_calib3d=OFF
        -DBUILD_opencv_features2d=OFF
        -DBUILD_opencv_java=OFF
        -DBUILD_opencv_objdetect=OFF
        -DBUILD_opencv_photo=OFF
        -DBUILD_opencv_nonfree=OFF
        -DBUILD_opencv_ocl=OFF
        -DBUILD_opencv_stitching=OFF
        -DBUILD_opencv_superres=OFF
        -DBUILD_opencv_ts=OFF
        -DBUILD_opencv_videostab=OFF
        -DBUILD_SHARED_LIBS:BOOL=ON
        -DBUILD_TESTS:BOOL=OFF
        -DBUILD_PERF_TESTS:BOOL=OFF
        -DBUILD_opencv_contrib=OFF
        -DBUILD_WITH_CAROTENE=OFF
        -DCMAKE_BUILD_TYPE:STRING=Debug
        -DWITH_FFMPEG:BOOL=ON
        -DWITH_CUDA=OFF
        -DWITH_IPP:BOOL=OFF
        -DBUILD_PNG:BOOL=OFF #
        -DBUILD_JPEG:BOOL=OFF #
        -DBUILD_ZLIB:BOOL=OFF # 
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF
        -DBUILD_FAT_JAVA_LIB=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DEPENDENCIES_DIR}/OpenCV
    )

MESSAGE(STATUS "OpenCV install prefix: ${INSTALL_DEPENDENCIES_DIR}/OpenCV")

set(OpenCV_DIR ${INSTALL_DEPENDENCIES_DIR}/OpenCV)


#if(EXISTS "${CMAKE_BINARY_DIR}/share/OpenCV/OpenCVConfig.cmake")
#    include(${CMAKE_BINARY_DIR}/share/OpenCV/OpenCVConfig.cmake)
#    add_custom_target(rerun)
#else()
#    add_custom_target(rerun ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS opencv)
#endif()