set(LIB_NAME "chirp")
add_library(${LIB_NAME} STATIC)

add_subdirectory(src)

target_include_directories(${LIB_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

include(../deps/box3d.cmake)
include(../deps/flecs.cmake)
include(../deps/nuklear.cmake)
include(../deps/sdl3.cmake)
include(../deps/qoi.cmake)
include(../deps/cgltf.cmake)

target_link_libraries(${LIB_NAME} PRIVATE
	SDL3::SDL3
	box3d::box3d
	flecs_static
)

target_include_directories(${LIB_NAME} PRIVATE
	"${cgltf_SOURCE_DIR}"
	"${nuklear_SOURCE_DIR}"
	"${qoi_SOURCE_DIR}"
)

include(../cmake/resources.cmake)
target_resources(${LIB_NAME})
