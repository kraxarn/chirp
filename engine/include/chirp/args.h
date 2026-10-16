#pragma once

#include "chirp/map.h"

#include <SDL3/SDL_stdinc.h>

typedef map_t args_t;

args_t args_create();

void args_destroy(args_t args);

bool args_parse(args_t args, int argc, char **argv);

bool args_print_help(args_t args, const char *name);

bool args_add(args_t args, const char *name,
	const char *description, bool *value);

bool args_add_flag(args_t args, const char *name,
	const char *description, bool *value);

bool args_add_string(args_t args, const char *name,
	const char *description, char **value);

bool args_add_int(args_t args, const char *name,
	const char *description, Sint64 *value);
