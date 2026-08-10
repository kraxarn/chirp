#include "ecs.h"
#include "gpudriver.h"
#include "gpushaderformat.h"
#include "logcategory.h"
#include "systeminfo.h"
#include "ecs/components.h"

#include "flecs.h"
#include "flecs/addons/flecs_c.h"

#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>

static void log_info(ecs_iter_t *iter)
{
	SDL_GPUDevice *device = *ecs_field(iter, gpu_device_t*, 0);

	SDL_LogInfo(LOG_CATEGORY_CORE, "Platform: %s",
		system_info_platform());

	SDL_LogInfo(LOG_CATEGORY_CORE, "CPU: %s",
		system_info_cpu_name());

	SDL_LogInfo(LOG_CATEGORY_CORE, "GPU: %s (%s)",
		system_info_gpu_name(device), system_info_gpu_driver(device));

	SDL_LogInfo(LOG_CATEGORY_CORE, "GPU driver: %s (%s)",
		SDL_GetGPUDeviceDriver(device), gpu_driver_names());

	SDL_LogInfo(LOG_CATEGORY_CORE, "Shader formats: %s",
		shader_format_names(device));

	SDL_LogInfo(LOG_CATEGORY_CORE, "Video driver: %s",
		SDL_GetCurrentVideoDriver());

	SDL_LogInfo(LOG_CATEGORY_CORE, "Audio driver: %s",
		SDL_GetCurrentAudioDriver());
}

void ecs_add_logging()
{
	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsGpuDevice), .inout = EcsInOut}
		},
		.events = {EcsOnSet},
		.callback = log_info,
		.yield_existing = true,
	});
}
