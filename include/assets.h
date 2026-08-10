#pragma once

#include "input.h"
#include "windowconfig.h"

#include "chirp/map.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_surface.h>

// TODO: Maybe move to ecs? (and remove map_t)
typedef struct assets
{
	SDL_IOStream *stream;
	window_config_t window_config;
	map_t desc;
} assets_t;

typedef struct model model_t;

bool assets_create(const char *path, input_t input, assets_t *assets);

void assets_destroy(const assets_t *assets);

[[nodiscard]]
window_config_t assets_window_config(const assets_t *assets);

[[nodiscard]]
SDL_IOStream *assets_load(const assets_t *assets, const char *name);

[[nodiscard]]
SDL_Surface *assets_load_texture(const assets_t *assets, const char *name);

[[nodiscard]]
bool assets_load_model(const assets_t *assets, SDL_GPUDevice *device, const char *name, model_t *model);

[[nodiscard]]
SDL_IOStream *assets_load_script(const assets_t *assets, const char *name);
