#https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1
#example slide 24
#https://github.com/boostcon/cppnow_presentations_2017/blob/master/05-19-2017_friday/effective_cmake__daniel_pfeifer__cppnow_05-19-2017.pdf

message(STATUS BOOST_ROOT ${BOOST_ROOT})
find_package (Boost 1.67.0 REQUIRED system program_options log)
ADD_DEFINITIONS(-BOOST_ALL_DYN_LINK)

message(STATUS OpenCV_DIR ${OpenCV_DIR})
find_package (OpenCV 3.4.1 REQUIRED)

message(STATUS ${_opencv_args})

add_library(Core
		src/CodecEvalFramework.cpp
		src/NamedTimer.cpp
		src/QuadTreeCodecFactory.cpp
		src/CodecEvalFramework.cpp
		src/DelegatingFactory.cpp include/cvHelpers.h)

target_include_directories(Core
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

message(STATUS ${Boost_LIBRARIES})

target_link_libraries(Core
		PUBLIC ${Boost_LIBRARIES}
		PUBLIC ${OpenCV_LIBS}
		-lboost_system
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

add_executable(TestCore test/TestCore.cpp)

target_include_directories(TestCore
		SYSTEM PRIVATE ${Boost_INCLUDE_DIRS}
		PUBLIC Core
		)
target_link_libraries(TestCore
		PUBLIC Core
		)

add_test(TestCore TestCore)