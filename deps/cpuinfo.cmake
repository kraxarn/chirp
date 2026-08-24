include(FetchContent)

FetchContent_Declare(cpuinfo
	GIT_REPOSITORY https://github.com/pytorch/cpuinfo.git
	GIT_TAG 66ee79c038d70dad9f08705b2c9b3e58f6d8f512
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
