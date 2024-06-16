
set(SOURCES
    client.cpp
)

add_library(msgrpc_client ${SOURCES})

target_include_directories(msgrpc_client PUBLIC
    ${CMAKE_SOURCE_DIR}/msgrpc
    ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include/
)

if (BUILD_CLIENT_TRANSPORT)
    add_subdirectory(transport)
    target_link_libraries(msgrpc_client PRIVATE msgrpc_client_transport)
endif()

target_link_libraries(msgrpc_client PRIVATE msgrpc_core)
