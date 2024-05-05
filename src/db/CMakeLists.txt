add_library(db STATIC
    file_db_manager.cpp
)

target_include_directories(db PRIVATE
    "${CMAKE_SOURCE_DIR}/src/"
)