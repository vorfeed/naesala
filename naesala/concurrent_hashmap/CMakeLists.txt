
#aux_source_directory(. concurrent_hashmap_SRCS)
#add_library(naesala_concurrent_hashmap ${concurrent_hashmap_SRCS})
#target_link_libraries(naesala_concurrent_hashmap pthread rt)
#set_target_properties(naesala_concurrent_hashmap PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_concurrent_hashmap DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/naesala/concurrent_hashmap)
