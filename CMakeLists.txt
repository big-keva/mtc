cmake_minimum_required(VERSION 3.10)
project(mtc)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()
add_compile_options(-mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wcast-align -pedantic -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wno-suggest-attribute=format)

add_library(mtc STATIC
  src/sharedLibrary.cpp
  src/threadPool.cpp
  src/byteBuffer.cpp
  src/fileStream.cpp
  src/bufStream.cpp
  src/zipStream.cpp
  src/config.cpp
  src/zval.cpp
  src/zmap.cpp
  src/json.cpp

  src/test-it-easy.cpp
)

install(TARGETS mtc DESTINATION lib)

install(
  FILES
    sharedLibrary.hpp

    utf.hpp
    wcsstr.h

    serialize.h

    interfaces.h
    iBuffer.h
    iStream.h

    byteBuffer.h
    fileStream.h
    zipStream.h

    charstream.h

    arena.hpp

    arbitrarymap.h
    patricia.h

    test-it-easy.hpp

    bitset.h
    directory.h
    file.h
    json.h
    platform.h
    ptrpatch.h
    recursive_shared_mutex.hpp
    strptime.h
    uuid.h
    config.h
    zmap.h
    zmap-alias.h
  DESTINATION include/mtc
)

# add_subdirectory(tutorial)

enable_testing()
add_subdirectory(tests)

