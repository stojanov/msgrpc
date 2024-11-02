project(msgrpc-transport)

set(SOURCES
    tcp_transport.cpp
)

find_package(asio CONFIG REQUIRED)

add_library(msgrpc_transport ${SOURCES})

target_include_directories(msgrpc_transport PRIVATE
    ${CMAKE_SOURCE_DIR}/msgrpc
    asio
)

target_link_libraries(msgrpc_transport PRIVATE asio::asio)

