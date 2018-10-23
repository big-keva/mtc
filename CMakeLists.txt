cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
if(MSVC)
  add_compile_options(-MP)
else()
  add_compile_options(-W -Wall -Wextra)
endif()

add_library(mtc STATIC
  byteBuffer.cpp
  dirtools.cpp
  fileStream.cpp
  jsconfig.cpp
  netStream.cpp
  zipStream.cpp
  config.cpp
  zval.cpp
  zmap.cpp
  z_js.cpp
)

install(TARGETS mtc DESTINATION lib)
install(
  FILES
    arbitrarymap.h
    array.h
    autoptr.h
    bitset.h
    byteBuffer.h
    charstream.h
    directory.h
    dirtools.hpp
    file.h
    fileStream.h
    http.h
    iBuffer.h
    interfaces.h
    iStream.h
    jsonTools.h
    netListen.h
    netStream.h
    patricia.h
    platform.h
    prefixtree.h
    serialize.h
    serializex.h
    sort.h
    stdlog.h
    strptime.h
    stringmap.h
    suballocator.h
    wcsstr.h
    zarray.h
    config.h
    zmap.h
    z_js.h
    zipStream.h
  DESTINATION include/mtc
)
