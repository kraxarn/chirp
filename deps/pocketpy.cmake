include(FetchContent)

FetchContent_Declare(pocketpy
	GIT_REPOSITORY https://github.com/pocketpy/pocketpy.git
	GIT_TAG 02b922a93eccb42d6d714f10e4539ad53b766ae1)

message(STATUS "Downloading: pocketpy")
FetchContent_MakeAvailable(pocketpy)

target_link_libraries(${PROJECT_NAME} PRIVATE pocketpy)