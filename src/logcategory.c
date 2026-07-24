#include "logcategory.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

const char *log_category_name(const int category)
{
	switch (category)
	{
		case SDL_LOG_CATEGORY_APPLICATION: return "app";
		case SDL_LOG_CATEGORY_ERROR: return "error";
		case SDL_LOG_CATEGORY_ASSERT: return "assert";
		case SDL_LOG_CATEGORY_SYSTEM: return "system";
		case SDL_LOG_CATEGORY_AUDIO: return "audio";
		case SDL_LOG_CATEGORY_VIDEO: return "video";
		case SDL_LOG_CATEGORY_RENDER: return "render";
		case SDL_LOG_CATEGORY_INPUT: return "input";
		case SDL_LOG_CATEGORY_TEST: return "test";
		case SDL_LOG_CATEGORY_GPU: return "gpu";
		case LOG_CATEGORY_CORE: return "core";
		case LOG_CATEGORY_FONT: return "font";
		case LOG_CATEGORY_ASSETS: return "assets";
		case LOG_CATEGORY_INPUT: return "input";
		case LOG_CATEGORY_PHYSICS: return "physics";
		case LOG_CATEGORY_MODEL: return "model";
		case LOG_CATEGORY_ECS: return "ecs";
		case LOG_CATEGORY_SCRIPT: return "script";
		case LOG_CATEGORY_UI: return "ui";
		default: return "unknown";
	}
}

int log_category_parse(const char *name)
{
	// SDL

	if (SDL_strcmp(name, "application") == 0)
	{
		return SDL_LOG_CATEGORY_APPLICATION;
	}

	if (SDL_strcmp(name, "error") == 0)
	{
		return SDL_LOG_CATEGORY_ERROR;
	}

	if (SDL_strcmp(name, "assert") == 0)
	{
		return SDL_LOG_CATEGORY_ASSERT;
	}

	if (SDL_strcmp(name, "system") == 0)
	{
		return SDL_LOG_CATEGORY_SYSTEM;
	}

	if (SDL_strcmp(name, "audio") == 0)
	{
		return SDL_LOG_CATEGORY_AUDIO;
	}

	if (SDL_strcmp(name, "video") == 0)
	{
		return SDL_LOG_CATEGORY_VIDEO;
	}

	if (SDL_strcmp(name, "render") == 0)
	{
		return SDL_LOG_CATEGORY_RENDER;
	}

	if (SDL_strcmp(name, "input") == 0)
	{
		return SDL_LOG_CATEGORY_INPUT;
	}

	if (SDL_strcmp(name, "test") == 0)
	{
		return SDL_LOG_CATEGORY_TEST;
	}

	if (SDL_strcmp(name, "gpu") == 0)
	{
		return SDL_LOG_CATEGORY_GPU;
	}

	// Custom

	if (SDL_strcmp(name, "core") == 0)
	{
		return LOG_CATEGORY_CORE;
	}

	if (SDL_strcmp(name, "font") == 0)
	{
		return LOG_CATEGORY_FONT;
	}

	if (SDL_strcmp(name, "assets") == 0)
	{
		return LOG_CATEGORY_ASSETS;
	}

	if (SDL_strcmp(name, "input") == 0)
	{
		return LOG_CATEGORY_INPUT;
	}

	if (SDL_strcmp(name, "physics") == 0)
	{
		return LOG_CATEGORY_PHYSICS;
	}

	if (SDL_strcmp(name, "model") == 0)
	{
		return LOG_CATEGORY_MODEL;
	}

	if (SDL_strcmp(name, "ecs") == 0)
	{
		return LOG_CATEGORY_ECS;
	}

	if (SDL_strcmp(name, "script") == 0)
	{
		return LOG_CATEGORY_SCRIPT;
	}

	if (SDL_strcmp(name, "ui") == 0)
	{
		return LOG_CATEGORY_UI;
	}

	return -1;
}
