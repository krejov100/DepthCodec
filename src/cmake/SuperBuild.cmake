
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
set( DepthCodec_DEPENDENCIES )

####################
###############Depnds
####################
option( USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)

	set(git_protocol "git")
	if(NOT USE_GIT_PROTOCOL)
        set(git_protocol "http")
	endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Boost.cmake)
set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} Boost )
list (APPEND EXTRA_CMAKE_ARGS
	-DBOOST_ROOT=${BOOST_ROOT}
	-DBoost_NO_SYSTEM_PATHS=ON
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-OpenCV.cmake)
set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} OpenCV )
list (APPEND EXTRA_CMAKE_ARGS
	-DOpenCV_DIR=${OpenCV_DIR}
	-DCMAKE_PREFIX_PATH=${OpenCV_DIR} 
)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/External-Open3D.cmake)
set( DepthCodec_DEPENDENCIES ${DepthCodec_DEPENDENCIES} Open3D )
list (APPEND EXTRA_CMAKE_ARGS
	-DOpen3D_ROOT=${Open3D_ROOT}
)

MESSAGE(STATUS "Now Configuring DepthCodec for Superbuild")
MESSAGE(STATUS "EXTRA_CMAKE_ARGS: ${EXTRA_CMAKE_ARGS}")

#Pass in to the DepthCodec the variables that are found by either find_package or created by External-"Depnd"
ExternalProject_Add (DepthCodec
		DEPENDS ${DepthCodec_DEPENDENCIES}
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src
        #CMAKE_GENERATOR ${gen}
        CMAKE_ARGS 
		-DUSE_SUPERBUILD=OFF 
		-DINSTALL_DEPENDENCIES_DIR=${INSTALL_DEPENDENCIES_DIR}
		${EXTRA_CMAKE_ARGS}
		INSTALL_COMMAND ""
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/DepthCodec
        )