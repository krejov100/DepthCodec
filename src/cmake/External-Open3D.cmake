message(STATUS "External project: Open3D")
		
ExternalProject_Add(Open3D
        GIT_REPOSITORY ${git_protocol}://github.com/IntelVCL/Open3D
        SOURCE_DIR Open3d
        BINARY_DIR open3d-build
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS ${ep_common_args}
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DEPENDENCIES_DIR}/thirdparty
    )

set(OPEN3D_INCLUDE_DIR ${CMAKE_BINARY_DIR}/thirdparty/include/)
if(NOT WIN32)
    set(OPEN3D_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/lib/)
else()
    set(OPEN3D_LIBRARY_DIR ${CMAKE_BINARY_DIR}/thirdparty/x86/vc12/lib)
endif()

#set(OPEN3D_LIBRARIES opencv_imgproc opencv_core opencv_highgui opencv_video opencv_videoio opencv_imgcodecs opencv_features2d)
if(EXISTS "${CMAKE_BINARY_DIR}/thirdparty/share/Open3d/Open3dConfig.cmake")
    include(${CMAKE_BINARY_DIR}/thirdparty/share/Open3d/Open3dConfig.cmake)
    add_custom_target(rerunOpen3D)
else()
    add_custom_target(rerunOpen3D ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR} DEPENDS Open3D)
endif()