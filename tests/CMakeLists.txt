cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)

link_libraries(mtc dl pthread)

if(FALSE)
	add_executable(mtc-test-buffer
		test-buffer.cpp test-main.cpp)
	add_executable(mtc-test-fileStream
        	test-fileStream.cpp test-main.cpp)
	add_executable(mtc-test-serialize
		test-serialize.cpp test-main.cpp)
	add_executable(mtc-test-patricia
		test-patricia.cpp test-main.cpp)
	add_executable(mtc-test-config
        	test-config.cpp test-main.cpp)
	add_executable(mtc-test-zmap
        	test-zmap.cpp test-main.cpp)
	add_executable(mtc-test-wcsstr
        	test-wcsstr.cpp test-main.cpp)
	add_executable(mtc-test-sharedMutex
		test-sharedMutex.cpp test-main.cpp)

	add_test(NAME mtc-test-patricia COMMAND test-patricia)
	add_test(NAME mtc-test-config COMMAND test-config)
	add_test(NAME mtc-test-zmap COMMAND test-zmap)
else()
	add_executable(mtc-test test-main.cpp
		test-buffer.cpp
        	test-fileStream.cpp
		test-serialize.cpp
		test-patricia.cpp
        	test-config.cpp
        	test-zmap.cpp
        	test-wcsstr.cpp
		test-sharedMutex.cpp)

	add_test(NAME mtc-test COMMAND test)
endif()
