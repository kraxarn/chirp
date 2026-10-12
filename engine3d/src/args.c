#include "args.h"

#include "chirp/logcategory.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include <stddef.h>

typedef void (*arg_callback_t)(char *buffer, size_t buffer_len);

typedef struct
{
	arg_type_t type;
	const char *command;
	arg_callback_t values;
	const char *description;
} arg_command_t;

static void log_priority_values(char *buffer, const size_t buffer_len)
{
	SDL_snprintf(buffer, buffer_len, "debug/info/warn/error");
}

static void video_driver_values(char *buffer, const size_t buffer_len)
{
	SDL_snprintf(buffer, buffer_len, "%s", SDL_GetVideoDriver(0));
	for (int i = 1; i < SDL_GetNumVideoDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetVideoDriver(i), buffer_len);
	}
}

static void audio_driver_values(char *buffer, const size_t buffer_len)
{
	SDL_snprintf(buffer, buffer_len, "%s", SDL_GetAudioDriver(0));
	for (int i = 1; i < SDL_GetNumAudioDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetAudioDriver(i), buffer_len);
	}
}

static void gpu_driver_values(char *buffer, const size_t buffer_len)
{
	SDL_snprintf(buffer, buffer_len, "%s", SDL_GetGPUDriver(0));
	for (int i = 0; i < SDL_GetNumGPUDrivers(); i++)
	{
		SDL_strlcat(buffer, "/", buffer_len);
		SDL_strlcat(buffer, SDL_GetGPUDriver(i), buffer_len);
	}
}

static void thread_values(char *buffer, const size_t buffer_len)
{
	const int cpu_cores = SDL_GetNumLogicalCPUCores();
	SDL_snprintf(buffer, buffer_len, "1-%d", cpu_cores);
}

static void task_thread_values(char *buffer, const size_t buffer_len)
{
	const int cpu_cores = SDL_GetNumLogicalCPUCores();
	SDL_snprintf(buffer, buffer_len, "1-%d", cpu_cores);
}

[[nodiscard]]
static arg_command_t *commands(size_t *len)
{
	const arg_command_t items[] = {
		(arg_command_t){
			.type = TYPE_NONE,
			.command = "help",
			.description = "Show this"
		},
		(arg_command_t){
			.type = TYPE_FLAG,
			.command = "prefer-low-power",
			.description = "Prefer integrated GPU over dedicated GPU",
		},
		(arg_command_t){
			.type = TYPE_FLAG,
			.command = "gpu-debug-mode",
			.description = "Enable GPU validations and more verbose GPU logging",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "log-priority",
			.values = log_priority_values,
			.description = "Set global logging verbosity",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "video-driver",
			.values = video_driver_values,
			.description = "Force specific video driver",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "audio-driver",
			.values = audio_driver_values,
			.description = "Force specific audio driver",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "gpu-driver",
			.values = gpu_driver_values,
			.description = "Force specific GPU driver",
		},
		(arg_command_t){
			.type = TYPE_FLAG,
			.command = "allow-screensaver",
			.description = "Enable screensaver while running",
		},
		(arg_command_t){
			.type = TYPE_FLAG,
			.command = "fatal-error-message-box",
			.description = "Show a message box on fatal error",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "threads",
			.values = thread_values,
			.description = "Set number of threads to use",
		},
		(arg_command_t){
			.type = TYPE_VALUE,
			.command = "task-threads",
			.values = task_thread_values,
			.description = "Set number of task threads to use",
		},
	};

	*len = SDL_arraysize(items);
	arg_command_t *result = SDL_calloc(*len, sizeof(arg_command_t));
	SDL_memcpy(result, items, *len * sizeof(arg_command_t));

	return result;
}

static void print_help(const arg_command_t *commands, const size_t size)
{
	// To avoid custom log formatting
	// (no need to clean-up since we should exit immediately afterwards)
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, "");
	const SDL_LogOutputFunction log_func = SDL_GetDefaultLogOutputFunction();

	log_func(nullptr, SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
		"Usage: " ENGINE_NAME " [options]");

	int command_length = 0;
	for (size_t i = 0; i < size; i++)
	{
		command_length = SDL_max(command_length, SDL_strlen(commands[i].command));
	}

	constexpr size_t buffer_len = 256;
	char buffer[buffer_len];

	for (size_t i = 0; i < size; i++)
	{
		const arg_command_t cmd = commands[i];

		SDL_snprintf(buffer, buffer_len, "  %s%-*s %s",
			cmd.type == TYPE_FLAG ? "--(no-)" : "--",
			command_length + (cmd.type == TYPE_FLAG ? 0 : 5),
			cmd.command, cmd.description
		);

		if (cmd.type == TYPE_VALUE)
		{
			const size_t len = SDL_strlcat(buffer, " [", buffer_len);
			cmd.values(buffer + len, buffer_len - len);
			SDL_strlcat(buffer, "]", buffer_len);
		}

		log_func(nullptr, SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, buffer);
	}
}

[[nodiscard]]
static SDL_LogPriority parse_log_priority(const char *name)
{
	// There are technically more categories,
	// but these are the "main ones" I feel

	if (SDL_strcmp(name, "debug") == 0)
	{
		return SDL_LOG_PRIORITY_DEBUG;
	}

	if (SDL_strcmp(name, "info") == 0)
	{
		return SDL_LOG_PRIORITY_INFO;
	}

	if (SDL_strcmp(name, "warn") == 0)
	{
		return SDL_LOG_PRIORITY_WARN;
	}

	if (SDL_strcmp(name, "error") == 0)
	{
		return SDL_LOG_PRIORITY_ERROR;
	}

	return SDL_LOG_PRIORITY_INVALID;
}

bool args_parse(const int argc, char **argv, args_t *args)
{
	*args = (args_t){
		.prefer_low_power = OPT_NOT_SET,
		.gpu_debug_mode = OPT_NOT_SET,
		.log_priorities = {0},
		.video_driver = nullptr,
		.audio_driver = nullptr,
		.gpu_driver = nullptr,
		.threads = 0,
		.task_threads = 0,
	};

	for (int i = 1; i < argc; i++)
	{
		const char *arg = argv[i];

		if (SDL_strcmp(arg, "--help") == 0)
		{
			size_t items_size = 0;
			arg_command_t *items = commands(&items_size);
			print_help(items, items_size);
			SDL_free(items);
			return false;
		}

		if (SDL_strcmp(arg, "--prefer-low-power") == 0)
		{
			args->prefer_low_power = OPT_ENABLE;
		}
		else if (SDL_strcmp(arg, "--no-prefer-low-power") == 0)
		{
			args->prefer_low_power = OPT_DISABLE;
		}

		else if (SDL_strcmp(arg, "--gpu-debug-mode") == 0)
		{
			args->gpu_debug_mode = OPT_ENABLE;
		}
		else if (SDL_strcmp(arg, "--no-gpu-debug-mode") == 0)
		{
			args->gpu_debug_mode = OPT_DISABLE;
		}

		else if (SDL_strcmp(arg, "--log-priority") == 0 && i + 1 < argc)
		{
			const char *value = argv[++i];

			char *sep = SDL_strchr(value, '=');
			if (sep == nullptr)
			{
				// Global log priority
				const SDL_LogPriority log_priority = parse_log_priority(value);
				if (log_priority == SDL_LOG_PRIORITY_INVALID)
				{
					SDL_LogError(LOG_CATEGORY_CORE, "Unknown priority: '%s'", argv[i]);
					continue;
				}
				args->log_priorities[LOG_CATEGORY_COUNT] = log_priority;
			}
			else
			{
				// Category log priority
				*sep = '\0';
				const int category = log_category_parse(value);
				if (category < 0)
				{
					SDL_LogError(LOG_CATEGORY_CORE, "Unknown category: '%s'", value);
					continue;
				}
				const SDL_LogPriority log_priority = parse_log_priority(sep + 1);
				if (log_priority == SDL_LOG_PRIORITY_INVALID)
				{
					SDL_LogError(LOG_CATEGORY_CORE, "Unknown priority: '%s'", argv[i]);
					continue;
				}
				args->log_priorities[category] = log_priority;
			}
		}

		else if (SDL_strcmp(arg, "--video-driver") == 0 && i + 1 < argc)
		{
			args->video_driver = argv[++i];
		}

		else if (SDL_strcmp(arg, "--audio-driver") == 0 && i + 1 < argc)
		{
			args->audio_driver = argv[++i];
		}

		else if (SDL_strcmp(arg, "--gpu-driver") == 0 && i + 1 < argc)
		{
			args->gpu_driver = argv[++i];
		}

		else if (SDL_strcmp(arg, "--allow-screensaver") == 0)
		{
			args->allow_screensaver = OPT_ENABLE;
		}
		else if (SDL_strcmp(arg, "--no-allow-screensaver") == 0)
		{
			args->allow_screensaver = OPT_DISABLE;
		}

		else if (SDL_strcmp(arg, "--fatal-error-message-box") == 0)
		{
			args->fatal_error_message_box = OPT_ENABLE;
		}
		else if (SDL_strcmp(arg, "--no-fatal-error-message-box") == 0)
		{
			args->fatal_error_message_box = OPT_DISABLE;
		}

		else if (SDL_strcmp(arg, "--worker-threads") == 0 && i + 1 < argc)
		{
			args->threads = SDL_atoi(argv[++i]);
		}
		else if (SDL_strcmp(arg, "--worker-task-threads") == 0 && i + 1 < argc)
		{
			args->task_threads = SDL_atoi(argv[++i]);
		}

		else
		{
			SDL_LogError(LOG_CATEGORY_CORE, "Unknown arg: '%s'", arg);
		}
	}

	return true;
}

const char *arg_option_str(const arg_option_t option)
{
	switch (option)
	{
		case OPT_NOT_SET:
			return "-1";

		case OPT_DISABLE:
			return "0";

		case OPT_ENABLE:
			return "1";

		default:
			return "";
	}
}
