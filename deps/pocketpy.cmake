include(FetchContent)

FetchContent_Declare(pocketpy
	GIT_REPOSITORY https://github.com/pocketpy/pocketpy.git
	GIT_TAG v2.1.3
)

set(PK_ENABLE_OS ON)
set(PK_ENABLE_THREADS ON)
set(PK_ENABLE_DETERMINISM OFF)
set(PK_ENABLE_WATCHDOG OFF)
set(PK_ENABLE_CUSTOM_SNAME OFF)
set(PK_ENABLE_MIMALLOC OFF)

set(PK_BUILD_MODULE_LZ4 OFF)
set(PK_BUILD_MODULE_LIBHV OFF)
set(PK_BUILD_MODULE_CUTE_PNG OFF)

message(STATUS "Downloading: pocketpy")
FetchContent_MakeAvailable(pocketpy)

target_link_libraries(${PROJECT_NAME} PRIVATE pocketpy)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${pocketpy_SOURCE_DIR}/include"
)