
#aux_source_directory(. log_SRCS)
#add_library(naesala_log ${log_SRCS})
#target_link_libraries(naesala_log pthread rt)
#set_target_properties(naesala_log PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_log DESTINATION lib)

#file(GLOB HEADERS "*.h")
#install(FILES ${HEADERS} DESTINATION include/naesala/log)
