include(FetchContent)

FetchContent_Declare(cpuinfo
	GIT_REPOSITORY https://github.com/pytorch/cpuinfo.git
	GIT_TAG d53799f29aee624d8c6b99bbe9b4a5846ae82588
)

message(STATUS "Downloading cpuinfo")

set(BUILD_SHARED_LIBS OFF)
set(CPUINFO_BUILD_TOOLS OFF)
set(CPUINFO_BUILD_UNIT_TESTS OFF)
set(CPUINFO_BUILD_MOCK_TESTS OFF)
set(CPUINFO_BUILD_BENCHMARKS OFF)
set(CPUINFO_BUILD_PKG_CONFIG OFF)
set(USE_SYSTEM_LIBS ON)

FetchContent_MakeAvailable(cpuinfo)
