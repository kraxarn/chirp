#include "chirp/resources.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>

static const Uint8 font_cousine_ttf[] =
{
#embed FONT_COUSINE_TTF_PATH
};

SDL_IOStream *res_font_cousine_ttf()
{
	return SDL_IOFromConstMem(font_cousine_ttf, sizeof(font_cousine_ttf));
}
