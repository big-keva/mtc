# Install script for directory: /Users/user/mtc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/user/mtc/build/libmtc.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtc.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtc.a")
    execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmtc.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mtc" TYPE FILE FILES
    "/Users/user/mtc/arbitrarymap.h"
    "/Users/user/mtc/array.h"
    "/Users/user/mtc/autoptr.h"
    "/Users/user/mtc/bitset.h"
    "/Users/user/mtc/byteBuffer.h"
    "/Users/user/mtc/charstream.h"
    "/Users/user/mtc/directory.h"
    "/Users/user/mtc/dirtools.hpp"
    "/Users/user/mtc/file.h"
    "/Users/user/mtc/fileStream.h"
    "/Users/user/mtc/http.h"
    "/Users/user/mtc/iBuffer.h"
    "/Users/user/mtc/interfaces.h"
    "/Users/user/mtc/iStream.h"
    "/Users/user/mtc/jsonTools.h"
    "/Users/user/mtc/netListen.h"
    "/Users/user/mtc/netStream.h"
    "/Users/user/mtc/patricia.h"
    "/Users/user/mtc/platform.h"
    "/Users/user/mtc/prefixtree.h"
    "/Users/user/mtc/serialize.h"
    "/Users/user/mtc/serializex.h"
    "/Users/user/mtc/sort.h"
    "/Users/user/mtc/stdlog.h"
    "/Users/user/mtc/strptime.h"
    "/Users/user/mtc/stringmap.h"
    "/Users/user/mtc/suballocator.h"
    "/Users/user/mtc/wcsstr.h"
    "/Users/user/mtc/zarray.h"
    "/Users/user/mtc/zipStream.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/user/mtc/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
