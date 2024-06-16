cmake_minimum_required(VERSION 3.23)
project(msgrpc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 20)

option(BUILD_CONSOLE "Build the test console application" ON) 
option(BUILD_CLIENT_TRANSPORT "Build the built-in client transport, currently only TCP" ON)
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." ON)

include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

add_subdirectory(msgrpc)

message("BUILD_CONSOLE ${BUILD_CONSOLE}")
message("BUILD_CLIENT_TRANSPORT ${BUILD_CLIENT_TRANSPORT}")

if (BUILD_CONSOLE)
    add_subdirectory(tests/console)
endif()

if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()
