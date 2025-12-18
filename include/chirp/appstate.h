#pragma once

#include "chirp/assets.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

typedef struct app_state_t
{
	assets_t *assets;

	SDL_Window *window;
	SDL_Renderer *renderer;
} app_state_t;
