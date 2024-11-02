add_subdirectory(core)
add_subdirectory(client)
add_subdirectory(system)

add_library(msgrpc INTERFACE)

if (BUILD_TRANSPORT)
    # add_subdirectory(transport)
    # target_link_libraries(msgrpc INTERFACE msgrpc_transport)
endif()

target_link_libraries(msgrpc INTERFACE msgrpc_client msgrpc_core msgrpc_system)
 
target_include_directories(msgrpc 
    INTERFACE
    ./
)

