project(core)

set(SOURCES
   call_handler.cpp 
   call_handler_base.cpp
    )

add_library(msgrpc_core STATIC ${SOURCES})

find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(msgrpc_core PRIVATE 
    spdlog::spdlog_header_only
    fmt::fmt-header-only
)

target_include_directories(msgrpc_core PRIVATE
    ${CMAKE_SOURCE_DIR}/msgrpc
    ${CMAKE_SOURCE_DIR}/vendor/cppack/msgpack/include/
    ./
)
