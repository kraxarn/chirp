cmake_minimum_required(VERSION 3.22)
project(chirp LANGUAGES CXX VERSION 0.0.1)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Engine
add_library(${PROJECT_NAME})
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include")

# Definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC CHIRP_VERSION="${PROJECT_VERSION}")

# Engine dependencies
include(deps/raylib.cmake)
include(deps/doctest.cmake)
include(deps/msgpack.cmake)
target_link_libraries(${PROJECT_NAME} PRIVATE
	raylib)

# Editor
add_executable(${PROJECT_NAME}-editor)
add_subdirectory(editor)
target_link_libraries(${PROJECT_NAME}-editor PRIVATE ${PROJECT_NAME})