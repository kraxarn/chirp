#pragma once

#include <SDL3/SDL_stdinc.h>

// TODO: Remove _ suffix

typedef struct args args_t_;

args_t_ *args_create();

void args_destroy(args_t_ *args);

bool args_parse_(const args_t_ *args, int argc, char **argv);

bool args_add(const args_t_ *args, const char *name,
	const char *description, bool *value);

bool args_add_flag(const args_t_ *args, const char *name,
	const char *description, bool *value);

bool args_add_string(const args_t_ *args, const char *name,
	const char *description, char **value);

bool args_add_int(const args_t_ *args, const char *name,
	const char *description, Sint64 *value);
