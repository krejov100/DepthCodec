#https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1
#example slide 24
#https://github.com/boostcon/cppnow_presentations_2017/blob/master/05-19-2017_friday/effective_cmake__daniel_pfeifer__cppnow_05-19-2017.pdf

add_library(Core
    )
target_include_directories(Core 
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	PRIVATE ${Boost_INCLUDE_DIR}
)

target_link_libraries(Core 
        PRIVATE ${Boost_LIBRARIES}
)


install(TARGETS Core EXPORT CoreTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT CoreTargets
    FILE CoreTargets.cmake
    NAMESPACE Syn::
    DESTINATION lib/cmake/Core
    )
