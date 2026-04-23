cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

add_executable(redis ${SOURCE_FILES}
        src/main.cpp
        src/server.cpp
        src/server.h
        src/client.h
        src/resp_utls.cpp
        src/resp_utls.h
        src/command_dir/command.h
        src/command_dir/command.cpp
        src/command_dir/list_command.h
        src/command_dir/list_command.cpp
        src/client.cpp
        src/global.cpp
        src/global.h
        src/command_dir/stream_command.cpp
        src/command_dir/stream_command.h
)

target_link_libraries(redis PRIVATE asio asio::asio)
target_link_libraries(redis PRIVATE Threads::Threads)