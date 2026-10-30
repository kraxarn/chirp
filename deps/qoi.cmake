include(FetchContent)

FetchContent_Declare(qoi
	GIT_REPOSITORY https://github.com/phoboslab/qoi.git
	GIT_TAG ffb2d2cb74a1de60819b21b939f7209aa53e91c1
)

message(STATUS "Downloading qoi")
FetchContent_MakeAvailable(qoi)
