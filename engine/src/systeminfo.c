#include "chirp/systeminfo.h"

#include "cpuinfo.h"

#include <SDL3/SDL_stdinc.h>

#ifdef SIMD_ENABLED
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_error.h>
#endif

#include <stddef.h>

static constexpr size_t name_length = 64;

const char *system_info_cpu_name()
{
	static char cpu_name[name_length];
	if (cpu_name[0] != '\0')
	{
		return cpu_name;
	}

	if (!cpuinfo_initialize())
	{
		return nullptr;
	}

	SDL_strlcpy(cpu_name, cpuinfo_get_package(0)->name, name_length);

	cpuinfo_deinitialize();
	return cpu_name;
}

const char *system_info_arch_name()
{
#if CPUINFO_ARCH_X86
	return "x86";
#elif CPUINFO_ARCH_X86_64
	return "x86_64";
#elif CPUINFO_ARCH_ARM
	return "arm";
#elif CPUINFO_ARCH_ARM64
	return "arm64";
#elif CPUINFO_ARCH_PPC64
	return "ppc64";
#elif CPUINFO_ARCH_WASM
	return "wasm";
#elif CPUINFO_ARCH_WASMSIMD
	return "wasm_simd";
#elif CPUINFO_ARCH_RISCV32
	return "riscv32";
#elif CPUINFO_ARCH_RISCV64
	return "riscv64";
#else
	return nullptr;
#endif
}

bool system_info_cpu_supported()
{
#ifdef SIMD_ENABLED
#if CPUINFO_ARCH_WASM == 1
	return true;
#elif CPUINFO_ARCH_X86_64 == 1 || CPUINFO_ARCH_X86 == 1
	if (!SDL_HasSSE2())
	{
		return SDL_SetError("CPU doesn't support required SSE2 features");
	}
#elif CPUINFO_ARCH_ARM64 == 1 || CPUINFO_ARCH_ARM == 1
	if (!SDL_HasNEON())
	{
		return SDL_SetError("CPU doesn't support required NEON features");
	}
#else
#error Unknown CPU architecture
#endif

	return true;
#else
	return true;
#endif
}
