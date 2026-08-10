#pragma once

#include "assets.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_surface.h>

[[nodiscard]]
SDL_Surface *assets_load_texture(const assets_t *assets, const char *name);

[[nodiscard]]
bool assets_load_model(const assets_t *assets, SDL_GPUDevice *device,
	const char *name, model_t *model);

[[nodiscard]]
SDL_IOStream *assets_load_script(const assets_t *assets, const char *name);
