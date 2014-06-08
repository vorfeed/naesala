
aux_source_directory(. benchmark_SRCS)
add_library(benchmark ${benchmark_SRCS})

target_link_libraries(
	benchmark
	pthread rt
	boost_thread
	boost_system
	boost_filesystem
	boost_date_time
	boost_program_options
	boost_regex
)

add_executable(blocking_queue main_blocking_queue.cpp)
target_link_libraries(blocking_queue benchmark)

add_executable(hashmap main_hashmap.cpp)
target_link_libraries(hashmap benchmark)

add_executable(concurrent_hashmap main_concurrent_hashmap.cpp)
target_link_libraries(concurrent_hashmap benchmark)

add_executable(lockfree main_lockfree.cpp)
target_link_libraries(lockfree benchmark)

add_executable(lockfree_pool main_lockfree_pool.cpp)
target_link_libraries(lockfree_pool benchmark)
