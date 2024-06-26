include(FetchContent)

FetchContent_Declare(pocketpy
	GIT_REPOSITORY https://github.com/pocketpy/pocketpy.git
	GIT_TAG v1.4.6)

message(STATUS "Downloading: pocketpy")
FetchContent_MakeAvailable(pocketpy)

target_link_libraries(${PROJECT_NAME} PRIVATE pocketpy)