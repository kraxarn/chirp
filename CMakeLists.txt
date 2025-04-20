cmake_minimum_required(VERSION 3.22)
project(chirp LANGUAGES C VERSION 1.90.0)

# C23
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# macOS config
if (APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "13")
endif ()

# Engine
add_library(${PROJECT_NAME} SHARED)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
	CHIRP_VERSION="${PROJECT_VERSION}"
)

# Engine dependencies
