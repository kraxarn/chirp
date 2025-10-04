include(FetchContent)

FetchContent_Declare(tomlc17
	GIT_REPOSITORY https://github.com/cktan/tomlc17.git
	GIT_TAG R250501x
)

message(STATUS "Downloading: tomlc17")
FetchContent_MakeAvailable(tomlc17)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${tomlc17_SOURCE_DIR}/src"
)

target_sources(${PROJECT_NAME} PRIVATE
	"${tomlc17_SOURCE_DIR}/src/tomlc17.c"
)