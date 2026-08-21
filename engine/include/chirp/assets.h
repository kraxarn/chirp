#pragma once

#include "chirp/input.h"
#include "chirp/map.h"
#include "chirp/windowconfig.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_mutex.h>

typedef struct assets
{
	SDL_IOStream *stream;
	SDL_Mutex *read_mutex;
	window_config_t window_config;
	map_t desc;
} assets_t;

bool assets_create(const char *path, input_t input, assets_t *assets);

void assets_destroy(const assets_t *assets);

[[nodiscard]]
window_config_t assets_window_config(const assets_t *assets);

[[nodiscard]]
SDL_IOStream *assets_load(const assets_t *assets, const char *name);
