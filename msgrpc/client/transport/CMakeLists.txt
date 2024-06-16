project(msgrpc-client-transport)

set(SOURCES
    tcp_transport.cpp 
)

find_package(asio CONFIG REQUIRED)

add_library(msgrpc_client_transport STATIC ${SOURCES})

target_include_directories(msgrpc_client_transport PRIVATE
    ${CMAKE_SOURCE_DIR}/msgrpc)

target_link_libraries(msgrpc_client_transport PRIVATE asio::asio)

