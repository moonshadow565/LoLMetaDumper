cmake_minimum_required (VERSION 3.1)

project(LoLMetaDumper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

add_executable(LoLMetaDumperExternal main_external.cpp)
add_library(LoLMetaDumperInternal SHARED main_internal.cpp)
add_library(BugSplat SHARED main_loader.cpp)
