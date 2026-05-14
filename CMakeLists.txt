cmake_minimum_required(VERSION 3.16)

project(HttpServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(server
    main.cpp
    SingleFileHttpServer.cpp
    Redis/RedisMainServer.cpp
    Redis/RedisProtocolParser.cpp
)

if(APPLE)
    target_compile_definitions(server PRIVATE _DARWIN_C_SOURCE)
endif()