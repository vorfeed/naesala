
#aux_source_directory(. lockfree_pool_SRCS)
#add_library(naesala_lockfree_pool ${lockfree_pool_SRCS})
#target_link_libraries(naesala_lockfree_pool pthread rt)
#set_target_properties(naesala_lockfree_pool PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_lockfree_pool DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/lockfree_pool)
