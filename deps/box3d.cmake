include(FetchContent)

FetchContent_Declare(box3d
	GIT_REPOSITORY https://github.com/erincatto/box3d.git
	# Box3D is tagged, but still very experimental, so use latest commit for now
	GIT_TAG 5d83df83ab47172c2c745b44315e7538ead02397
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
