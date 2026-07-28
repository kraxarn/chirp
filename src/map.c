#include "map.h"

#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

#include <inttypes.h>
#include <stddef.h>

static void cleanup(void *userdata, void *value)
{
	((map_cleanup_callback_t) userdata)(value);
}

const char *map_key_str(const char *key)
{
	return key;
}

#define key_str(name, fmt, key)									\
	static constexpr size_t arrsize = (2 * sizeof(key)) + 1;	\
	static char (name)[arrsize];								\
	SDL_snprintf((name), arrsize, "%"fmt, key);

const char *map_key_uint8(const Uint8 key)
{
	key_str(temp, PRIx8, key);
	return temp;
}

const char *map_key_uint16(const Uint16 key)
{
	key_str(temp, PRIx16, key);
	return temp;
}

const char *map_key_uint32(const Uint32 key)
{
	key_str(temp, PRIx32, key);
	return temp;
}

const char *map_key_uint64(const Uint64 key)
{
	key_str(temp, PRIx64, key);
	return temp;
}

bool map_set_with_cleanup(const map_t map, const char *name, void *value,
	const map_cleanup_callback_t callback)
{
	return SDL_SetPointerPropertyWithCleanup(map, name, value, cleanup, callback);
}
