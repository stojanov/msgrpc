
set(SOURCES
    async_pool.cpp
)

add_library(msgrpc_system ${SOURCES})

target_include_directories(msgrpc_system PUBLIC
    ${CMAKE_SOURCE_DIR}/msgrpc
)

