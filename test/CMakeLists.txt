
aux_source_directory(. test_SRCS)
add_library(test ${test_SRCS})

target_link_libraries(
	test
	pthread rt
	boost_thread
	boost_system
	boost_filesystem
	boost_date_time
	boost_program_options
	boost_regex
)

add_executable(proc_stat main_proc_stat.cpp)
target_link_libraries(proc_stat test)
add_executable(sizeof main_sizeof.cpp)
target_link_libraries(sizeof test)
