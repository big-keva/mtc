project(mtc-tests)

cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

link_libraries(mtc dl pthread)

add_library(mtc-test-sharedLibrary-lib SHARED
	test-sharedLibrary-lib.cpp)

add_compile_definitions(
	CMAKE_BINARY_DIR=${PROJECT_BINARY_DIR})

if(TRUE)
	add_executable(mtc-test-bitset
		test-bitset.cpp test-main.cpp)
	add_executable(mtc-test-buffer
		test-buffer.cpp test-main.cpp)
	add_executable(mtc-test-fileStream
        	test-fileStream.cpp test-main.cpp)
	add_executable(mtc-test-serialize
		test-serialize.cpp test-main.cpp)
	add_executable(mtc-test-patricia
		test-patricia.cpp test-main.cpp)
	add_executable(mtc-test-radix-tree
		test-radix-tree.cpp test-main.cpp)
	add_executable(mtc-test-config
        	test-config.cpp test-main.cpp)
	add_executable(mtc-test-zmap
        	test-zmap.cpp test-main.cpp)
	add_executable(mtc-test-wcsstr
        	test-wcsstr.cpp test-main.cpp)
	add_executable(mtc-test-sharedMutex
		test-sharedMutex.cpp test-main.cpp)
	add_executable(mtc-test-sharedLibrary
		test-sharedLibrary.cpp test-main.cpp)

	add_dependencies(mtc-test-sharedLibrary
		mtc-test-sharedLibrary-lib)

	add_test(NAME mtc-test-patricia COMMAND test-patricia)
	add_test(NAME mtc-test-radix-tree COMMAND test-patricia)
	add_test(NAME mtc-test-config COMMAND test-config)
	add_test(NAME mtc-test-zmap COMMAND test-zmap)

else()
	add_executable(mtc-test test-main.cpp
		test-bitset.cpp
		test-buffer.cpp
        	test-fileStream.cpp
		test-serialize.cpp
		test-patricia.cpp
		test-radix-tree.cpp
        	test-config.cpp
        	test-zmap.cpp
        	test-wcsstr.cpp
		test-sharedMutex.cpp
		test-sharedLibrary.cpp)

	add_test(NAME mtc-test COMMAND mtc-test)
endif()
