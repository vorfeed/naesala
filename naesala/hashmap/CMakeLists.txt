
#aux_source_directory(. hashmap_SRCS)
#add_library(naesala_hashmap ${hashmap_SRCS})
#target_link_libraries(naesala_hashmap pthread rt)
#set_target_properties(naesala_hashmap PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_hashmap DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/hashmap)
