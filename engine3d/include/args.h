#pragma once

#include <SDL3/SDL_stdinc.h>

typedef enum : Uint8
{
	PARSE_RESULT_SUCCESS,
	PARSE_RESULT_ERROR,
	PARSE_RESULT_EXIT,
} arg_parse_result_t;

typedef struct
{
	/**
	 * --prefer-low-power / --no-prefer-low-power
	 *
	 * Create the GPU device with the "prefer low power" hint
	 */
	bool prefer_low_power;

	/**
	 * --gpu-debug-mode / --no-gpu-debug-mode
	 *
	 * Create the GPU device with the "debug mode" and "verbose" hints
	 */
	bool gpu_debug_mode;

	/**
	 * --log-priority debug/info/warn/error
	 *
	 * --log-priority category=debug/info/warn/error
	 *
	 * Set level of all log categories or within a specific log category only
	 */
	char *log_priorities;

	/**
	 * --video-driver wayland/x11/...
	 *
	 * Override the default video driver with the specified one
	 */
	char *video_driver;

	/**
	 * --audio-driver pipewire/pulseaudio/...
	 *
	 * Override the default audio driver with the specified one
	 */
	char *audio_driver;

	/**
	 * --gpu-driver vulkan/metal/direct3d12
	 *
	 * Override the default GPU driver with the specified one
	 */
	char *gpu_driver;

	/**
	 * --allow-screensaver / --no-allow-screensaver
	 *
	 * Set the "allow screensaver" hint
	 */
	bool allow_screensaver;

	/**
	 * --fatal-error-message-box / --no-fatal-error-message-box
	 *
	 * Show a message box on fatal error
	 */
	bool fatal_error_message_box;

	/**
	 * --ecs-threading threads/tasks
	 *
	 * Use threads or task threads for ECS
	 */
	char *ecs_threading;

	/**
	 * --ecs-threads 0-...
	 *
	 * Set number of ECS threads
	 */
	Sint64 ecs_threads;

	/**
	 * --physics-threads 0-...
	 *
	 * Set number of physics threads/workers
	 */
	Sint64 physics_threads;
} arg_values_t;

[[nodiscard]]
arg_parse_result_t arg_values_parse(int argc, char **argv, arg_values_t *values);
