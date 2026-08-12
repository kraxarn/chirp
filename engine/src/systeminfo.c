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

bool system_info_cpu_supported()
{
#ifdef SIMD_ENABLED
#if CPUINFO_ARCH_X86_64 == 1 || CPUINFO_ARCH_X86 == 1
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
