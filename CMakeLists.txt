cmake_minimum_required(VERSION 3.10)
project(mtc)

option(INSTALL_MTC "Enable mtc installation" OFF)
option(BUILD_TESTS "Build library tests" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
  add_compile_options(-MP -D_CRT_SECURE_NO_WARNINGS)
else()
  add_compile_options(-fPIC)
  add_compile_options(-pedantic -mfpmath=sse
    -W -Wall -Wextra -Wcast-align -Wfloat-equal -Wpointer-arith -Wformat-security
    -Wmissing-format-attribute -Wformat=1 -Wwrite-strings -Wcast-align -Wno-long-long
    -Woverloaded-virtual -Wno-suggest-attribute=format)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-fno-stack-protector -D_FORTIFY_SOURCE=0 -fno-builtin)
  else()
    add_compile_options(-fstack-protector-all -g)
  endif()
endif()

add_library(mtc STATIC
  src/sharedLibrary.cpp
  src/threadPool.cpp
  src/byteBuffer.cpp
  src/fileStream.cpp
  src/bufStream.cpp
  src/zipStream.cpp
  src/config.cpp
  src/zval.cpp
  src/zval-math.cpp
  src/zval-cast.cpp
  src/zval-compare.cpp
  src/zmap.cpp
  src/json.cpp

  src/test-it-easy.cpp
)

if(INSTALL_MTC)

  install(TARGETS mtc DESTINATION lib)

  install(FILES
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
  DESTINATION include/mtc)
endif()

if(BUILD_TESTS)
  enable_testing()
endif()
