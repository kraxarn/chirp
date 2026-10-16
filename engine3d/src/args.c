#include "args.h"

#include "chirp/args.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

bool arg_values_parse(const int argc, char **argv, arg_values_t *values)
{
	*values = (arg_values_t){
		.prefer_low_power = false,
		.log_priorities = nullptr,
		.video_driver = nullptr,
		.audio_driver = nullptr,
		.gpu_driver = nullptr,
		.allow_screensaver = false,
		.ecs_threading = "threads",
		.ecs_threads = 0,
		.physics_threads = 0,
#ifdef NDEBUG
		.gpu_debug_mode = false,
		.fatal_error_message_box = true,
#else
		.gpu_debug_mode = true,
		.fatal_error_message_box = false,
#endif
	};

	bool help = false;
	const args_t args = args_create();

	args_add(args,
		"help",
		"Show this",
		&help
	);
	args_add_flag(args,
		"prefer-low-power",
		"Prefer integrated GPU over dedicated GPU",
		&values->prefer_low_power
	);
	args_add_flag(args,
		"gpu-debug-mode",
		"Enable GPU validations and more verbose GPU logging",
		&values->gpu_debug_mode
	);
	args_add_string(args,
		"log-priority",
		"Set global logging verbosity [debug/info/warn/error]",
		&values->log_priorities
	);
	args_add_flag(args,
		"allow-screensaver",
		"Enable screensaver while running",
		&values->allow_screensaver
	);
	args_add_flag(args,
		"fatal-error-message-box",
		"Show a message box on fatal error",
		&values->fatal_error_message_box
	);
	args_add_string(args,
		"ecs-threading",
		"Set threading type for ECS [threads/tasks]",
		&values->ecs_threading
	);

	constexpr size_t buffer_len = 256;
	char buffer[buffer_len];

	SDL_snprintf(buffer, buffer_len, "Override default video driver [%s",
		SDL_GetVideoDriver(0));
	for (int i = 1; i < SDL_GetNumVideoDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetVideoDriver(i), buffer_len);
	}
	SDL_strlcat(buffer, "]", buffer_len);
	args_add_string(args, "video-driver", buffer, &values->video_driver);

	SDL_snprintf(buffer, buffer_len, "Override default audio driver [%s",
		SDL_GetAudioDriver(0));
	for (int i = 1; i < SDL_GetNumAudioDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetAudioDriver(i), buffer_len);
	}
	SDL_strlcat(buffer, "]", buffer_len);
	args_add_string(args, "audio-driver", buffer, &values->audio_driver);

	SDL_snprintf(buffer, buffer_len, "Override default GPU driver [%s",
		SDL_GetGPUDriver(0));
	for (int i = 1; i < SDL_GetNumGPUDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetGPUDriver(i), buffer_len);
	}
	SDL_strlcat(buffer, "]", buffer_len);
	args_add_string(args, "gpu-driver", buffer, &values->gpu_driver);

	const int cpu_cores = SDL_GetNumLogicalCPUCores();

	SDL_snprintf(buffer, buffer_len, "Number of threads to use for ECS [1-%d]", cpu_cores);
	args_add_int(args, "ecs-threads", buffer, &values->ecs_threads);

	SDL_snprintf(buffer, buffer_len, "Number of threads to use for physics [1-%d]", cpu_cores);
	args_add_int(args, "physics-threads", buffer, &values->physics_threads);

	if (!args_parse(args, argc, argv))
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
			"Parse error: %s", SDL_GetError());
	}

	if (help)
	{
		args_print_help(args, ENGINE_NAME);
		args_destroy(args);
		return false;
	}

	args_destroy(args);
	return true;
}
