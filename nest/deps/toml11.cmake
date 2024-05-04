include(FetchContent)

FetchContent_Declare(toml11
	GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
	GIT_TAG v3.8.1)

message(STATUS "Downloading: toml11")
FetchContent_MakeAvailable(toml11)

target_link_libraries(${PROJECT_NAME} PRIVATE toml11)