cmake_minimum_required(VERSION 3.22)
project(chirp3d LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_TESTS "Build unit tests" OFF)

if (ANDROID)
	add_library(${PROJECT_NAME} SHARED)
else ()
	add_executable(${PROJECT_NAME})
endif ()

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	ENGINE_NAME="${PROJECT_NAME}"
	ENGINE_VERSION="${PROJECT_VERSION}"
	ENGINE_IDENTIFIER="com.kraxarn.${PROJECT_NAME}"
	ENGINE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
	ENGINE_COMPILER="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"
)

set(ENABLE_SIMD_DEFAULT ON)
if (ANDROID AND CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
	set(ENABLE_SIMD_DEFAULT OFF)
endif ()
option(ENABLE_SIMD "Enable SSE2/NEON instructions" ${ENABLE_SIMD_DEFAULT})
if (ENABLE_SIMD)
	target_compile_definitions(${PROJECT_NAME} PRIVATE SIMD_ENABLED)
endif ()

include(deps/sdl3.cmake)
include(deps/qoi.cmake)
include(deps/cpuinfo.cmake)
include(deps/cgltf.cmake)
include(deps/flecs.cmake)
include(deps/pocketpy.cmake)
include(deps/nuklear.cmake)
include(deps/box3d.cmake)

target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL3::SDL3
	box3d::box3d
	cpuinfo
	flecs_static
	pocketpy
)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${cgltf_SOURCE_DIR}"
	"${nuklear_SOURCE_DIR}"
	"${qoi_SOURCE_DIR}"
)

include(cmake/resources.cmake)
target_resources(${PROJECT_NAME})

if (ENABLE_TESTS)
	add_subdirectory(tests)
endif ()

include(cmake/execproperties.cmake)
set_target_exec_properties(${PROJECT_NAME}
	IDENTIFIER    "com.kraxarn.${PROJECT_NAME}"
	VERSION_LONG  "${PROJECT_VERSION}"
	VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

include(cmake/lto.cmake)
target_enable_lto(${PROJECT_NAME})

if (TARGET SDL3-shared)
	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		VERBATIM
	)
endif ()

# Not all platforms define "NDEBUG"
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif ()
