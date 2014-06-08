
#aux_source_directory(. blocking_queue_SRCS)
#add_library(naesala_blocking_queue ${blocking_queue_SRCS})
#target_link_libraries(naesala_blocking_queue pthread rt)
#set_target_properties(naesala_blocking_queue PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_blocking_queue DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/blocking_queue)
