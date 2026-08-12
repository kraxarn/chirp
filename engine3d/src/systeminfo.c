#include "systeminfo.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

static constexpr size_t name_length = 64;

const char *system_info_gpu_name(SDL_GPUDevice *device)
{
	static char gpu_name[name_length];
	if (gpu_name[0] != '\0')
	{
		return gpu_name;
	}

	const SDL_PropertiesID props = SDL_GetGPUDeviceProperties(device);

	const char *name = SDL_GetStringProperty(props,
		SDL_PROP_GPU_DEVICE_NAME_STRING, nullptr);

	if (name == nullptr)
	{
		return nullptr;
	}

	SDL_strlcpy(gpu_name, name, name_length);
	return gpu_name;
}

const char *system_info_gpu_driver(SDL_GPUDevice *device)
{
	static char gpu_driver_name[name_length];
	if (gpu_driver_name[0] != '\0')
	{
		return gpu_driver_name;
	}

	const SDL_PropertiesID props = SDL_GetGPUDeviceProperties(device);

	const char *name = SDL_GetStringProperty(props,
		SDL_PROP_GPU_DEVICE_DRIVER_NAME_STRING, nullptr);

	if (name == nullptr)
	{
		return nullptr;
	}

	SDL_strlcpy(gpu_driver_name, name, name_length);

	const char *version = SDL_GetStringProperty(props,
		SDL_PROP_GPU_DEVICE_DRIVER_INFO_STRING, SDL_GetStringProperty(props,
			SDL_PROP_GPU_DEVICE_DRIVER_VERSION_STRING, nullptr));

	if (version == nullptr)
	{
		return nullptr;
	}

	SDL_strlcat(gpu_driver_name, " ", name_length);
	SDL_strlcat(gpu_driver_name, version, name_length);
	return gpu_driver_name;
}
