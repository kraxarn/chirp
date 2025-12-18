#pragma once

#include <SDL3/SDL_log.h>

typedef enum log_category_t
{
	LOG_CATEGORY_ASSETS = SDL_LOG_CATEGORY_CUSTOM,
	LOG_CATEGORY_SCRIPT,
} log_category_t;
