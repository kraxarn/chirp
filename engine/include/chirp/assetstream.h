#pragma once

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_mutex.h>
#include <SDL3/SDL_stdinc.h>

[[nodiscard]]
SDL_IOStream *asset_stream_open_io(SDL_IOStream *stream,
	SDL_Mutex *read_mutex, Sint64 offset, Sint64 size);
