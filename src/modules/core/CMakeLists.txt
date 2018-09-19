#https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1
#example slide 24
#https://github.com/boostcon/cppnow_presentations_2017/blob/master/05-19-2017_friday/effective_cmake__daniel_pfeifer__cppnow_05-19-2017.pdf



find_package (OpenCV 3)
find_package(Boost COMPONENTS system program_options log serialization REQUIRED)
find_package(Open3D REQUIRED)


FOREACH(LIB ${Open3D_LIBRARIES})
	FIND_LIBRARY(FOUND_LIB_${LIB} ${LIB} HINT ${Open3D_LIBRARY_DIRS})
	LIST(APPEND Open3D_FULL_LIBRARIES ${FOUND_LIB_${LIB}})
	MESSAGE("Lib: ${LIB}")
	MESSAGE("Found Lib: ${FOUND_LIB_${LIB}}")
ENDFOREACH(LIB)

message(STATUS "FULL LIB PATHS ${Open3D_FULL_LIBRARIES}")


add_library(Core SHARED
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
		include/Marshaller
		include/BoostMarshaller
		include/ReadWrite
		include/ImageConversions.h
		include/DepthCodec.h
		src/NamedTimer.cpp
		src/DepthCodecFactory.cpp 
		src/QuadTreeCodecFactory.cpp
		src/CodecEvalFramework.cpp
		include/Frame.h include/PointCloudConversions.h src/ImageError.cpp)

target_include_directories(Core
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		SYSTEM PRIVATE ${OpenCV_INCLUDE_DIR}
		SYSTEM PRIVATE ${Open3D_INCLUDE_DIRS}
		#SYSTEM PRIVATE ${realsense_INCLUDE_DIR}
		SYSTEM PRIVATE ${librealsense_INCLUDE_DIRS}
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	)

target_link_libraries(Core
		PUBLIC -lpthread
		PUBLIC -fopenmp
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ${OpenCV_LIBS}
		PUBLIC ${Open3D_FULL_LIBRARIES}
		PUBLIC ${REALSENSE2_FOUND}
		#PUBLIC ${realsense2_LIBRARY}
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
		PUBLIC Core
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS}
		SYSTEM PRIVATE ${Open3D_INCLUDE_DIRS}
		#SYSTEM PRIVATE ${librealsense_INCLUDE_DIRS}
		#SYSTEM PRIVATE ${GTK3_INCLUDE_DIRS}
	)

target_link_libraries(TestCore
		PUBLIC Core
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ${OpenCV_LIBS}
		PUBLIC ${Open3D_FULL_LIBRARIES}
		#PUBLIC ${realsense2_LIBRARY}
		PUBLIC ${REALSENSE2_FOUND}
		#PUBLIC ${GTK3_LIBRARIES}
	)

add_test(TestCore TestCore)