cmake_minimum_required(VERSION 3.22)
project(chirp LANGUAGES C VERSION 1.90.0)

# C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# macOS config
if (APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
endif ()

# Engine
add_executable(${PROJECT_NAME})
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Don't link stdlib
# (force more consistent API from SDL)
#set_target_properties(${PROJECT_NAME} PROPERTIES
#	LINK_FLAGS "-nostdlib"
#)

# Definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CHIRP_VERSION="${PROJECT_VERSION}"
)

# Engine dependencies
include(deps/sdl3.cmake)
include(deps/pocketpy.cmake)