cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(test-patricia
	test-patricia.cpp)
