#pragma once

#include "chirp/input.h"
#include "chirp/map.h"
#include "chirp/windowconfig.h"

#include <SDL3/SDL_iostream.h>

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
