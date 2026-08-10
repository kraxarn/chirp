#pragma once

#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

typedef SDL_PropertiesID map_t;

#define map_create   SDL_CreateProperties
#define map_destroy  SDL_DestroyProperties
#define map_contains SDL_HasProperty

// Custom callback to directly support common "destroy" functions
typedef void (*map_cleanup_callback_t)(void *value);

const char *map_key_str(const char *key);
const char *map_key_uint8(Uint8 key);
const char *map_key_uint16(Uint16 key);
const char *map_key_uint32(Uint32 key);
const char *map_key_uint64(Uint64 key);
const char *map_key_sint32(Sint32 key);

#define map_key(value)					\
	_Generic((value),					\
		char*:        map_key_str,		\
		const char*:  map_key_str,		\
		Uint8:        map_key_uint8,	\
		Uint16:       map_key_uint16,	\
		Uint32:       map_key_uint32,	\
		Uint64:       map_key_uint64,	\
		Sint32:       map_key_sint32	\
	)(value)

#define map_get(map, name, fallback)		\
	_Generic((fallback),					\
		void*:      SDL_GetPointerProperty,	\
		nullptr_t:  SDL_GetPointerProperty,	\
		char*:      SDL_GetStringProperty,	\
		Sint64:     SDL_GetNumberProperty,	\
		int:        SDL_GetNumberProperty,	\
		Uint32:     SDL_GetNumberProperty,	\
		float:      SDL_GetFloatProperty,	\
		bool:       SDL_GetBooleanProperty	\
	)(map, map_key(name), fallback)

#define map_set(map, name, value)			\
	_Generic((value),						\
		void*:      SDL_SetPointerProperty,	\
		nullptr_t:  SDL_SetPointerProperty,	\
		char*:      SDL_SetStringProperty,	\
		Sint64:     SDL_SetNumberProperty,	\
		int:        SDL_SetNumberProperty,	\
		Uint32:     SDL_SetNumberProperty,	\
		float:      SDL_SetFloatProperty,	\
		bool:       SDL_SetBooleanProperty	\
	)(map, map_key(name), value)

bool map_set_with_cleanup(map_t map, const char *name,
	void *value, map_cleanup_callback_t callback);
