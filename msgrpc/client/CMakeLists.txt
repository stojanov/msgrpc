
set(SOURCES
    client.cpp
    async_runtime.cpp
)

add_library(msgrpc_client ${SOURCES})

target_include_directories(msgrpc_client PUBLIC
    ${CMAKE_SOURCE_DIR}/msgrpc
    ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include/
)

target_link_libraries(msgrpc_client PRIVATE msgrpc_core)
