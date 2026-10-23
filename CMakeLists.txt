cmake_minimum_required(VERSION 3.22)
project(chirp LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(ENABLE_SIMD_DEFAULT ON)
if (ANDROID AND CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
	set(ENABLE_SIMD_DEFAULT OFF)
endif ()
option(ENABLE_SIMD "Enable SSE2/NEON instructions" ${ENABLE_SIMD_DEFAULT})

if (EMSCRIPTEN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s GROWABLE_ARRAYBUFFERS=2")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s TOTAL_STACK=131072")
endif ()

include(deps/box3d.cmake)
include(deps/cgltf.cmake)
include(deps/cpuinfo.cmake)
include(deps/flecs.cmake)
include(deps/nuklear.cmake)
include(deps/sdl3.cmake)

add_subdirectory(engine)
add_subdirectory(engine3d)
add_subdirectory(tests)
