cmake_minimum_required(VERSION 3.1)
project(mtc)

set(CMAKE_C_STANDARD 14)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()
add_compile_options(-mfpmath=sse -fstack-protector-all -g -W -Wall -Wextra -Wcast-align -pedantic -Wfloat-equal -Wpointer-arith -Wformat-security -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long -Woverloaded-virtual -Wno-suggest-attribute=format)

add_library(mtc STATIC
  src/byteBuffer.cpp
  src/fileStream.cpp
  src/zipStream.cpp
  src/config.cpp
  src/zval.cpp
  src/zmap.cpp
  src/json.cpp

  src/test-it-easy.cpp

  dir.cpp

  net/sockets.cpp
)

install(TARGETS mtc DESTINATION lib)
install(
  FILES
    arbitrarymap.h
    arena.hpp
    bitset.h
    byteBuffer.h
    charstream.h
    dir.hpp
    directory.h
    file.h
    fileStream.h
    iBuffer.h
    interfaces.h
    iStream.h
    json.h
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
    zipStream.h
    test-it-easy.hpp
  DESTINATION include/mtc
)
install(
  FILES
    net/sockets.hpp
  DESTINATION include/mtc/net
)

add_subdirectory(tutorial)

# enable_testing()
# add_subdirectory(tests)

