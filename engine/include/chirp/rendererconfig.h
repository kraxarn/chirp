#pragma once

#include <SDL3/SDL_pixels.h>

typedef struct
{
	SDL_FColor clear_color;
} renderer_config_t;

[[nodiscard]]
renderer_config_t renderer_config_default();
