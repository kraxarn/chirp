include(FetchContent)

FetchContent_Declare(libcbor
	GIT_REPOSITORY https://github.com/PJK/libcbor.git
	# Latest release doesn't support CMake 4.0
	GIT_TAG 1183292d4695300785b272532c1e02d68840e4b8
)

set(WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SANITIZE OFF CACHE BOOL "" FORCE)

message(STATUS "Downloading: libcbor")
FetchContent_MakeAvailable(libcbor)

target_link_libraries(${PROJECT_NAME} PRIVATE cbor)