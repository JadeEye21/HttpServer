cmake_minimum_required(VERSION 3.16)

project(HttpServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optional: compile_commands.json for clangd / IDE
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(server
    main.cpp
    hdelibc.cpp
    Servers/SimpleServer.cpp
    Servers/TestServer.cpp
    Sockets/SimpleSocket.cpp
    Sockets/BindingSocket.cpp
    Sockets/ListeningSocket.cpp
    Sockets/ConnectingSocket.cpp
)

target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Sockets
    ${CMAKE_CURRENT_SOURCE_DIR}/Servers
)

# macOS / Linux socket APIs
if(APPLE)
    target_compile_definitions(server PRIVATE _DARWIN_C_SOURCE)
endif()
