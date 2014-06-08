cmake_minimum_required(VERSION 2.6)

project(naesala)

add_definitions(
	-w
	-pthread -pipe
	-ggdb3 -O3
	-std=c++11 -D__GXX_EXPERIMENTAL_CXX0X__
)

set(PROJ_ROOT ${PROJECT_SOURCE_DIR})

find_package(Boost REQUIRED)

include_directories(
	${PROJ_ROOT}
	${Boost_INCLUDE_DIRS}
)

add_subdirectory(naesala)
add_subdirectory(test)
add_subdirectory(benchmark)

