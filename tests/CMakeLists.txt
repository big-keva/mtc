cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(mtc-test
	test-arbitraryMap.cpp
	test-config.cpp
	test-directory.cpp
	test-fileStream.cpp
	test-patricia.cpp
	test-prefixTree.cpp
	test-serialize.cpp
	test-utf.cpp
	test-wcsstr.cpp

	test-main.cpp)

target_link_libraries(mtc-test
	mtc)

add_test(NAME mtc-test COMMAND test-mtc)
