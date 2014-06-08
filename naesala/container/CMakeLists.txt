
#aux_source_directory(. container_SRCS)
#add_library(naesala_container ${container_SRCS})
#target_link_libraries(naesala_container pthread rt)
#set_target_properties(naesala_container PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_container DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/container)
