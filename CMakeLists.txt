cmake_minimum_required(VERSION 3.28)
project(http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath=\$ORIGIN -Wl,-E")

add_subdirectory("./src/db")
add_subdirectory("./src/server")
add_subdirectory("./src/timer")
set(app_link_list db http_server timer)
add_executable(app src/main.cpp)

find_package(Boost COMPONENTS thread system REQUIRED)

target_include_directories(app PRIVATE "${CMAKE_SOURCE_DIR}/src/")
target_link_libraries(app PRIVATE
        ${Boost_LIBRARIES}
        ${app_link_list}
)
