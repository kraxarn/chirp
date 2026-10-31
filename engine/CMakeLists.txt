set(LIB_NAME "chirp")
add_library(${LIB_NAME} STATIC)

add_subdirectory(src)

target_include_directories(${LIB_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${LIB_NAME} PUBLIC
	SDL3::SDL3
	box3d::box3d
	flecs_static
)

target_link_libraries(${LIB_NAME} PRIVATE
	cpuinfo
)

target_include_directories(${LIB_NAME} PUBLIC
	"${nuklear_SOURCE_DIR}"
)

target_include_directories(${LIB_NAME} PRIVATE
	"${cgltf_SOURCE_DIR}"
	"${qoi_SOURCE_DIR}"
)

include(../cmake/resources.cmake)
target_resources(${LIB_NAME})

if (ENABLE_SIMD)
	target_compile_definitions(${LIB_NAME} PUBLIC SIMD_ENABLED)
endif ()

if (ENABLE_WARNINGS_AS_ERRORS)
	target_compile_options(${LIB_NAME} PUBLIC -Wall -Wextra -Wpedantic -Werror)
	if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${LIB_NAME} PUBLIC
			-Wno-gnu-pointer-arith				# void* arithmetics in arrays
			-Wno-cast-function-type-mismatch	# Some hacky casting in ECS threads
		)
	elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${LIB_NAME} PUBLIC
			-Wno-pointer-arith		# void* arithmetics in arrays
			-Wno-cast-function-type	# Some hacky casting in ECS threads
			-Wno-pedantic			# Map cleanup casts pointers to/from functions
		)
	endif ()
endif ()

# Not all platforms define "NDEBUG"
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(${LIB_NAME} PUBLIC NDEBUG)
endif ()
