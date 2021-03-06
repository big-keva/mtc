cmake_minimum_required(VERSION 3.5)
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

  dirtools.cpp
  netStream.cpp
  dir.cpp
  z_js.cpp

  net/sockets.cpp
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
    dir.hpp
    directory.h
    dirtools.hpp
    file.h
    fileStream.h
    http.h
    iBuffer.h
    interfaces.h
    iStream.h
    jsonTools.h
    json.h
    netListen.h
    netStream.h
    patricia.h
    platform.h
    prefixtree.h
    recursive_shared_mutex.hpp
    serialize.h
    serializex.h
    sort.h
    stdlog.h
    strptime.h
    stringmap.h
    suballocator.h
    utf.hpp
    uuid.h
    wcsstr.h
    zarray.h
    config.h
    zmap.h
    zmap-alias.h
    z_js.h
    zipStream.h
  DESTINATION include/mtc
)
install(
  FILES
    net/sockets.hpp
  DESTINATION include/mtc/net
)

# enable_testing()
# add_subdirectory(tests)

