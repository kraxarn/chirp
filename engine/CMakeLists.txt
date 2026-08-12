set(LIB_NAME "chirp")
add_library(${LIB_NAME} STATIC)

add_subdirectory(src)

target_include_directories(${LIB_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${LIB_NAME} PUBLIC
	SDL3::SDL3
)

target_link_libraries(${LIB_NAME} PRIVATE
	box3d::box3d
	cpuinfo
	flecs_static
)

target_include_directories(${LIB_NAME} PRIVATE
	"${cgltf_SOURCE_DIR}"
	"${nuklear_SOURCE_DIR}"
	"${qoi_SOURCE_DIR}"
)

include(../cmake/resources.cmake)
target_resources(${LIB_NAME})

if (ENABLE_SIMD)
	target_compile_definitions(${LIB_NAME} PUBLIC SIMD_ENABLED)
endif ()

if (TARGET SDL3-shared)
	add_custom_command(
		TARGET ${LIB_NAME} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${LIB_NAME}>
		VERBATIM
	)
endif ()

# Not all platforms define "NDEBUG"
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${LIB_NAME} PUBLIC NDEBUG)
endif ()
