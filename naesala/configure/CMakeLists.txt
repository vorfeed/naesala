
#aux_source_directory(. configure_SRCS)
#add_library(naesala_configure ${configure_SRCS})
#target_link_libraries(naesala_configure pthread rt)
#set_target_properties(naesala_configure PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_configure DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/configure)
