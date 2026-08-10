include(FetchContent)

FetchContent_Declare(cgltf
	GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
	GIT_TAG v1.15
)

message(STATUS "Downloading cgltf")
FetchContent_MakeAvailable(cgltf)
