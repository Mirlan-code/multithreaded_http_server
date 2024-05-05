add_library(http_server STATIC
        connection.cpp
        mime_types.cpp
        reply.cpp
        request_handler.cpp
        request_parser.cpp
        server.cpp
        total_stat_manager.cpp
)

find_package(Boost COMPONENTS thread system REQUIRED)
target_link_libraries(http_server PRIVATE
        ${Boost_LIBRARIES}
)

target_include_directories(http_server PRIVATE
        "${CMAKE_SOURCE_DIR}/src/"
        "${CMAKE_SOURCE_DIR}/external/rapidjson/include"
)
