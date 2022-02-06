cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(mtc-test-serialize
	test-serialize.cpp)

add_test(NAME test-serialize COMMAND test-serialize)
