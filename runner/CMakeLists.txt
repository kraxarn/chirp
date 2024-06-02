cmake_minimum_required(VERSION 3.22)
project(aviary LANGUAGES CXX VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})

add_subdirectory(src)

add_subdirectory(.. chirp)
target_link_libraries(${PROJECT_NAME} PRIVATE chirp)

include(../deps/pocketpy.cmake)