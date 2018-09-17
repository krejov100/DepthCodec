message(STATUS "External project: Open3D")
		
ExternalProject_Add(Open3D
		GIT_REPOSITORY ${git_protocol}://github.com/IntelVCL/Open3D
        SOURCE_DIR Open3D
		BINARY_DIR ${INSTALL_DEPENDENCIES_DIR}/Open3D
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CMAKE_GENERATOR ${gen}
        CMAKE_ARGS ${ep_common_args}
        -DCMAKE_INSTALL_PREFIX:PATH=${INSTALL_DEPENDENCIES_DIR}/Open3D
    )

MESSAGE(STATUS "Open3D install prefix: ${INSTALL_DEPENDENCIES_DIR}/Open3D/CMakeFiles")

set(Open3D_ROOT ${INSTALL_DEPENDENCIES_DIR}/Open3D/CMakeFiles)

