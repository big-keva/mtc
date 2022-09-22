cmake_minimum_required(VERSION 3.1)
project(mtc)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_library(mtc STATIC
  src/byteBuffer.cpp
  src/fileStream.cpp
  src/zipStream.cpp
  src/config.cpp
  src/zval.cpp
  src/zmap.cpp
  src/json.cpp

  src/test-it-easy.cpp)

install(TARGETS mtc DESTINATION lib)

install(
  FILES
    arbitrarymap.h
    bitset.h
    byteBuffer.h
    directory.h
    file.h
    fileStream.h
    iBuffer.h
    interfaces.h
    iStream.h
    patricia.h
    platform.h
    ptrpatch.h
    recursive_shared_mutex.hpp
    serialize.h
    strptime.h
    stringmap.h
    utf.hpp
    uuid.h
    wcsstr.h
    config.h
    zmap.h
    zmap-alias.h
    z_js.h
    zipStream.h
    test-it-easy.hpp
  DESTINATION include/mtc)

add_subdirectory(tutorial)

# enable_testing()
add_subdirectory(tests)
