cmake_minimum_required(VERSION 3.0)
project(server LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckCXXSourceCompiles)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}  "-g -std=c++20 -Wall")

add_executable(${PROJECT_NAME}  test_server.cpp Socket.cpp Server.cpp )









