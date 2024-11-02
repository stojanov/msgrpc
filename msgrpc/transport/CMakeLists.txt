project(msgrpc-transport)

set(SOURCES)

find_package(asio CONFIG REQUIRED)

# add_library(msgrpc_transport INTERFACE)
#
# target_include_directories(msgrpc_transport PRIVATE
#     ${CMAKE_SOURCE_DIR}/msgrpc
# )
#
# target_link_libraries(msgrpc_transport PRIVATE asio::asio)
#
