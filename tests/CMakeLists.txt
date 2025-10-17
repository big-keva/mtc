project(mtc-tests)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	link_libraries(mtc)
else()
	link_libraries(mtc dl pthread)
endif()

add_library(mtc-test-sharedLibrary-lib SHARED
	test-sharedLibrary-lib.cpp)

add_compile_definitions(
	CMAKE_BINARY_DIR=${PROJECT_BINARY_DIR}
	CMAKE_DLL_PREFIX=/${CMAKE_SHARED_LIBRARY_PREFIX}
	CMAKE_DLL_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX})

set(COMMON_SRC tmppath.cpp test-main.cpp)

if(FALSE)
	add_executable(mtc-test-bitset
		test-bitset.cpp ${COMMON_SRC})
	add_executable(mtc-test-buffer
		test-buffer.cpp ${COMMON_SRC})
	add_executable(mtc-test-fileStream
        	test-fileStream.cpp ${COMMON_SRC})
	add_executable(mtc-test-bufStream
        	test-bufStream.cpp ${COMMON_SRC})
	add_executable(mtc-test-serialize
		test-serialize.cpp ${COMMON_SRC})
	add_executable(mtc-test-patricia
		test-patricia.cpp ${COMMON_SRC})
	add_executable(mtc-test-radix-tree
		test-radix-tree.cpp ${COMMON_SRC})
	add_executable(mtc-test-config
        	test-config.cpp ${COMMON_SRC})
	add_executable(mtc-test-zmap
        	test-zmap.cpp ${COMMON_SRC})
	add_executable(mtc-test-wcsstr
        	test-wcsstr.cpp ${COMMON_SRC})
	add_executable(mtc-test-sharedMutex
		test-sharedMutex.cpp ${COMMON_SRC})
	add_executable(mtc-test-sharedLibrary
		test-sharedLibrary.cpp ${COMMON_SRC})
	add_executable(mtc-test-threadPool
		test-threadPool.cpp ${COMMON_SRC})
	add_executable(mtc-test-arbitrarymap
		test-arbitrarymap.cpp ${COMMON_SRC})

	add_dependencies(mtc-test-sharedLibrary
		mtc-test-sharedLibrary-lib)

	add_test(NAME mtc-test-patricia COMMAND test-patricia)
	add_test(NAME mtc-test-radix-tree COMMAND test-patricia)
	add_test(NAME mtc-test-config COMMAND test-config)
	add_test(NAME mtc-test-zmap COMMAND test-zmap)

else()
	add_executable(mtc-test
		test-arbitrarymap.cpp
		test-bitset.cpp
		test-buffer.cpp
        	test-fileStream.cpp
		test-bufStream.cpp
		test-serialize.cpp
		test-patricia.cpp
		test-radix-tree.cpp
        	test-config.cpp
        	test-zmap.cpp
        	test-wcsstr.cpp
		test-sharedMutex.cpp
		test-threadPool.cpp
		test-sharedLibrary.cpp
		${COMMON_SRC})

	add_dependencies(mtc-test
		mtc-test-sharedLibrary-lib)

	add_test(NAME mtc-test COMMAND mtc-test)
endif()
