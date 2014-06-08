
#aux_source_directory(. lockfree_SRCS)
#add_library(naesala_lockfree ${lockfree_SRCS})
#target_link_libraries(naesala_lockfree pthread rt)
#set_target_properties(naesala_lockfree PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_lockfree DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/lockfree)
