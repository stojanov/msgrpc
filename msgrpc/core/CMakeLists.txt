project(core)


set(SOURCES
   call_handler.cpp 
   call_handler_base.cpp
    )

add_library(msgrpc_core STATIC ${SOURCES})

target_include_directories(msgrpc_core PRIVATE
    ${CMAKE_SOURCE_DIR}/msgrpc
    ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include/
    ./
    )
