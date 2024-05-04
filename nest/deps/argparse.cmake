include(FetchContent)

FetchContent_Declare(argparse
	GIT_REPOSITORY https://github.com/p-ranav/argparse.git
	GIT_TAG v3.0)

message(STATUS "Downloading: argparse")
FetchContent_MakeAvailable(argparse)

target_link_libraries(${PROJECT_NAME} PRIVATE argparse)