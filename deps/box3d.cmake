include(FetchContent)

FetchContent_Declare(box3d
	GIT_REPOSITORY https://github.com/erincatto/box3d.git
	# Box3D is tagged, but still very experimental, so use latest commit for now
	GIT_TAG 3fc20f5b453ba9e14cdf54ecafa87a2a4bcdf53c
)

set(BOX3D_SANITIZE OFF)
set(BOX3D_COMPILE_WARNING_AS_ERROR OFF)
set(BOX3D_DOUBLE_PRECISION OFF)

if (CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
	set(BOX3D_DISABLE_SIMD ON)
else ()
	set(BOX3D_DISABLE_SIMD OFF)
endif ()


message(STATUS "Downloading box3d")
FetchContent_MakeAvailable(box3d)

target_link_libraries(${PROJECT_NAME} PRIVATE
	box3d::box3d
)
