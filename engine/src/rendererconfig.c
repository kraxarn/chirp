#include "chirp/rendererconfig.h"

#include <SDL3/SDL_pixels.h>

renderer_config_t renderer_config_default()
{
	return (renderer_config_t){
		.clear_color = (SDL_FColor){
			.r = 0.12F,
			.g = 0.12F,
			.b = 0.12F,
			.a = 1.F,
		},
	};
}
