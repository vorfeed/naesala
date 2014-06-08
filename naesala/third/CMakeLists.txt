
#aux_source_directory(. third_SRCS)
#aux_source_directory(ulib third_SRCS)
#add_library(naesala_third ${third_SRCS})
#target_link_libraries(naesala_third pthread rt)
#set_target_properties(naesala_third PROPERTIES COMPILE_FLAGS "-std=c++0x")
#install(TARGETS naesala_third DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/naesala/third)
file(GLOB ULIB_HEADERS "ulib/*.h")
install(FILES ${ULIB_HEADERS} DESTINATION include/naesala/third/ulib)
