

set(SOURCES 
    msgrpc.cpp
)



add_subdirectory(core)
add_subdirectory(client)

add_executable(msgrpc ${SOURCES})

target_link_libraries(msgrpc PUBLIC msgrpc_client)

target_include_directories(msgrpc
    PUBLIC
        ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include
        ./
)

