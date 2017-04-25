cmake_minimum_required(VERSION 3.5)
project(mtc)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-W -Wall -Wextra)

add_library(mtc STATIC
  byteBuffer.cpp
  dirtools.cpp
  fileStream.cpp
  jsconfig.cpp
  netStream.cpp
  zipStream.cpp
)
