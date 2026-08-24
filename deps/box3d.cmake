include(FetchContent)

FetchContent_Declare(box3d
	GIT_REPOSITORY https://github.com/erincatto/box3d.git
	# Box3D is tagged, but still very experimental, so use latest commit for now
	GIT_TAG 30c67b5e6d0a3a66f0f506c69ce9e9e0587e3b7c
)

set(BOX3D_SANITIZE OFF)
set(BOX3D_COMPILE_WARNING_AS_ERROR OFF)
set(BOX3D_DOUBLE_PRECISION OFF)

if (ENABLE_SIMD)
	set(BOX3D_DISABLE_SIMD OFF)
else ()
	set(BOX3D_DISABLE_SIMD ON)
endif ()

message(STATUS "Downloading box3d")
FetchContent_MakeAvailable(box3d)
