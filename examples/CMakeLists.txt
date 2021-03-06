file(GLOB EXAMPLES RELATIVE ${CMAKE_CURRENT_LIST_DIR} example_*.cpp)

if(STATICLIB_SWITCH)
	set(CMAKE_EXE_LINKER_FLAGS "-static")
	set(BUILD_SHARED_LIBS OFF)
endif()

foreach( example ${EXAMPLES} )
	string(REPLACE ".cpp" "" example_name ${example} )
    add_executable( ${example_name} EXCLUDE_FROM_ALL ${example})

    if(STATICLIB_SWITCH)
    	#set_target_properties( ${example_name} PROPERTIES LINK_SEARCH_END_STATIC TRUE )
    	set(CMAKE_EXE_LINKER_FLAGS "-static")
    	set(BUILD_SHARED_LIBS OFF)
    	target_link_libraries( ${example_name} ${PROJECT_NAME}_static)
    else()
		target_link_libraries( ${example_name} ${PROJECT_NAME})
    endif()
endforeach( example )

# ATTENTION: This is just used to test if all examples compile.
add_executable( allExamples EXCLUDE_FROM_ALL ${EXAMPLES} )
target_link_libraries( allExamples ${PROJECT_NAME})

if(${STATICLIB_SWITCH} STREQUAL "ON")
	set_target_properties( allExamples PROPERTIES LINK_SEARCH_END_STATIC TRUE )
endif()
