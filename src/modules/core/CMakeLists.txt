#https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1
#example slide 24
#https://github.com/boostcon/cppnow_presentations_2017/blob/master/05-19-2017_friday/effective_cmake__daniel_pfeifer__cppnow_05-19-2017.pdf

set(Boost_USE_STATIC_LIBS ON)

message(STATUS BOOST_ROOT ${BOOST_ROOT})
find_package (Boost COMPONENTS system program_options log serialization REQUIRED)
ADD_DEFINITIONS(-BOOST_ALL_DYN_LINK)

message(STATUS OpenCV_DIR ${OpenCV_DIR})
find_package (OpenCV 3.4.1 REQUIRED)

message(STATUS ${_opencv_args})

add_library(Core STATIC
		include/CodecEvalFramework.h
		include/DepthCodecFactory.h
		include/Factory.h
		include/IDepthCodec.h
		include/TiledCodec.h
		include/NamedTimer.h
		include/NodeAddress.h
		include/Tree.h
		include/QuadTree.h
		include/QuadTreeCodecFactory.h
		include/QuadTreeTypes.h
		include/RollingQuadTree.h
		include/Range.h
		src/NamedTimer.cpp
		src/DepthCodecFactory.cpp 
		src/QuadTreeCodecFactory.cpp
		src/CodecEvalFramework.cpp
	)

target_include_directories(Core
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	)

target_link_libraries(Core
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ${OpenCV_LIBS}
	)

install(TARGETS Core EXPORT CoreTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

install(EXPORT CoreTargets
	FILE CoreTargets.cmake
	DESTINATION lib/cmake/Core
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/resources/bgExampleDepth.tif  ${CMAKE_CURRENT_BINARY_DIR}/resources/bgExampleDepth.tif  COPYONLY)

add_executable(TestCore test/TestCore.cpp)

target_include_directories(TestCore
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		PUBLIC Core
	)
target_link_libraries(TestCore
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC Core
	)

add_test(TestCore TestCore)