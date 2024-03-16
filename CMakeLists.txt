cmake_minimum_required(VERSION 3.23)
project(msgrpc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)
set(BUILD_CONSOLE ON) 

add_subdirectory(msgrpc)

if (BUILD_CONSOLE)
    add_subdirectory(tests/console)
endif()
