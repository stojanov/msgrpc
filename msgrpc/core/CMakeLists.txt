project(base)


set(SOURCES
   call_handler.cpp 
   call_handler_base.cpp
    )

add_library(msgpack_core ${SOURCES})

target_include_directories(msgpack_core PRIVATE
    ${CMAKE_SOURCE_DIR}/msgrpc
    ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include/
    ./
    )
