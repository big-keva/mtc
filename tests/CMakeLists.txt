cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

add_executable(test-patricia
	test-patricia.cpp)

add_executable(test-directory
	test-directory.cpp)

add_executable(test-config
        test-config.cpp)

add_executable(test-zmap
        test-zmap.cpp)

add_executable(mtc-test-wcsstr
        test-wcsstr.cpp)

add_executable(mtc-test-fileStream
        test-fileStream.cpp)

target_link_libraries(test-directory
	mtc)
target_link_libraries(test-config
	mtc)
target_link_libraries(test-zmap
	mtc)
target_link_libraries(mtc-test-wcsstr
	mtc)
target_link_libraries(mtc-test-fileStream
	mtc)

add_test(NAME test-patricia COMMAND test-patricia)
add_test(NAME test-directory COMMAND test-directory)
add_test(NAME test-config COMMAND test-config)
add_test(NAME test-zmap COMMAND test-zmap)
