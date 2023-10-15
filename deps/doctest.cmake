include(FetchContent)

FetchContent_Declare(doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG v2.4.11)

message(STATUS "Downloading: doctest")
FetchContent_MakeAvailable(doctest)