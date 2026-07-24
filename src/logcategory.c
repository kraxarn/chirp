#include "logcategory.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

static const char *category_names[LOG_CATEGORY_COUNT] = {
	[ SDL_LOG_CATEGORY_APPLICATION] = "app",
	[ SDL_LOG_CATEGORY_ERROR] = "error",
	[ SDL_LOG_CATEGORY_ASSERT] = "assert",
	[ SDL_LOG_CATEGORY_SYSTEM] = "system",
	[ SDL_LOG_CATEGORY_AUDIO] = "audio",
	[ SDL_LOG_CATEGORY_VIDEO] = "video",
	[ SDL_LOG_CATEGORY_RENDER] = "render",
	[ SDL_LOG_CATEGORY_INPUT] = "input",
	[ SDL_LOG_CATEGORY_TEST] = "test",
	[ SDL_LOG_CATEGORY_GPU] = "gpu",
	[ LOG_CATEGORY_CORE] = "core",
	[ LOG_CATEGORY_FONT] = "font",
	[ LOG_CATEGORY_ASSETS] = "assets",
	[ LOG_CATEGORY_INPUT] = "input",
	[ LOG_CATEGORY_PHYSICS] = "physics",
	[ LOG_CATEGORY_MODEL] = "model",
	[ LOG_CATEGORY_ECS] = "ecs",
	[ LOG_CATEGORY_SCRIPT] = "script",
	[ LOG_CATEGORY_UI] = "ui",
};

const char *log_category_name(const int category)
{
	SDL_assert(category >= 0 && category < SDL_arraysize(category_names));
	return category_names[category];
}

int log_category_parse(const char *name)
{
	for (int i = 0; i < LOG_CATEGORY_COUNT; i++)
	{
		const char *category_name = category_names[i];
		if (category_name == nullptr)
		{
			continue;
		}

		if (SDL_strcmp(name, category_name) == 0)
		{
			return i;
		}
	}

	return -1;
}
