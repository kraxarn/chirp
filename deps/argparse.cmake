include(FetchContent)

FetchContent_Declare(argparse
	# Upstream source doesn't support CMake 4.0
	GIT_REPOSITORY https://github.com/Fexty12573/argparse.git
	GIT_TAG ebc26673e34458a9d717674e0ef449dc6dcd9ceb
)

message(STATUS "Downloading: argparse")
FetchContent_MakeAvailable(argparse)

target_link_libraries(${PROJECT_NAME} PRIVATE argparse_static)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${argparse_SOURCE_DIR}"
)