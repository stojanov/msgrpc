

set(SOURCES 
    msgrpc.cpp
)

add_subdirectory(core)
add_subdirectory(client)

add_library(msgrpc ${SOURCES})

target_link_libraries(msgrpc PUBLIC msgrpc_client msgrpc_core)

target_include_directories(msgrpc
    PUBLIC
        ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include
        ./
)

