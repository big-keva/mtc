cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

link_libraries(mtc)

add_executable(mtc-test-serialize
	test-serialize.cpp)

add_executable(mtc-test-patricia
	test-patricia.cpp
	test-main.cpp)

add_executable(mtc-test-directory
	test-directory.cpp)

add_executable(mtc-test-config
        test-config.cpp)

add_executable(mtc-test-zmap
        test-zmap.cpp)

add_executable(mtc-test-wcsstr
        test-wcsstr.cpp)

add_executable(mtc-test-fileStream
        test-fileStream.cpp)

add_executable(mtc-test-sharedMutex
	test-sharedMutex.cpp)

target_link_libraries(mtc-test-directory
	mtc)
target_link_libraries(mtc-test-config
	mtc)
target_link_libraries(mtc-test-zmap
	mtc)
target_link_libraries(mtc-test-wcsstr
	mtc)
target_link_libraries(mtc-test-fileStream
	mtc)
target_link_libraries(mtc-test-sharedMutex
	mtc
        pthread)

add_test(NAME test-patricia COMMAND test-patricia)
add_test(NAME test-directory COMMAND test-directory)
add_test(NAME test-config COMMAND test-config)
add_test(NAME test-zmap COMMAND test-zmap)
