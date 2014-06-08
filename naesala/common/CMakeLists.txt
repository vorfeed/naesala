
#aux_source_directory(. common_SRCS)
#add_library(naesala_common ${common_SRCS})
#target_link_libraries(naesala_common pthread rt)
#set_target_properties(naesala_common PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_common DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/common)
