cmake_minimum_required(VERSION 2.8)

project(mtc)

add_definitions( -Wall -std=c++1y -pedantic )

set(SOURCE_LIB byteBuffer.cpp  dirtools.cpp  fileStream.cpp  jsconfig.cpp  netStream.cpp  zipStream.cpp)

add_library(mtc STATIC ${SOURCE_LIB})
