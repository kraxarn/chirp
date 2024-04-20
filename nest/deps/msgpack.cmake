include(FetchContent)

FetchContent_Declare(msgpack
	GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
	GIT_TAG cpp-6.1.1)

set(MSGPACK_CXX20 ON CACHE BOOL "" FORCE)
set(MSGPACK_USE_BOOST OFF CACHE BOOL "" FORCE)

message(STATUS "Downloading: msgpack")
FetchContent_MakeAvailable(msgpack)

target_link_libraries(${PROJECT_NAME} PRIVATE msgpack-cxx)