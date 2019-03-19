cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(test-patricia
	test-patricia.cpp)

add_executable(test-directory
	test-directory.cpp)

target_link_libraries(test-directory
	mtc)

add_test(NAME test-patricia COMMAND test-patricia)
add_test(NAME test-directory COMMAND test-directory)

