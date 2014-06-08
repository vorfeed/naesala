
add_subdirectory(third)
add_subdirectory(log)
add_subdirectory(common)
add_subdirectory(configure)
add_subdirectory(blocking_queue)
add_subdirectory(hashmap)
add_subdirectory(concurrent_hashmap)
add_subdirectory(lockfree)
add_subdirectory(lockfree_pool)

#target_link_libraries(
#	naesala
#	naesala_third
#	naesala_log
#	naesala_common
#	naesala_configure
#	naesala_blocking_queue
#	naesala_hashmap
#	naesala_concurrent_hashmap
#	naesala_lockfree
#	naesala_lockfree_pool
#)

#set_target_properties(naesala PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/naesala)

