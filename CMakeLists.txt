cmake_minimum_required (VERSION 3.1)

project(LoLMetaDumper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
if(WIN32)
    if(MSVC)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libstdc++ -static-libgcc")
    endif()
endif()

add_executable(LoLMetaDumperExternal main_external.cpp)
add_library(LoLMetaDumperInternal SHARED main_internal.cpp)
add_library(BugSplat SHARED main_loader.cpp)
