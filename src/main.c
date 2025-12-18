#include "chirp/appstate.h"
#include "chirp/assets.h"
#include "chirp/scriptengine.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#ifdef NDEBUG
#include <SDL3/SDL_messagebox.h>
#endif

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

static SDL_AppResult fatal_error([[maybe_unused]] SDL_Window *window, const char *message)
{
	SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s: %s", message, SDL_GetError());
#ifdef NDEBUG
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, message, SDL_GetError(), window);
#endif
	return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit([[maybe_unused]] void **appstate, [[maybe_unused]]
	const int argc, [[maybe_unused]] char **argv)
{
#ifdef NDEBUG
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_WARN);
#else
	SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
#endif

	app_state_t *app_state = SDL_malloc(sizeof(app_state_t));
	if (app_state == nullptr)
	{
		return fatal_error(nullptr, "Memory allocation failed");
	}
	*appstate = app_state;

	app_state->assets = assets_create();
	if (app_state->assets == nullptr)
	{
		return fatal_error(nullptr, "Assets loading failed");
	}

	script_engine_create();
	char *main_py = assets_load(app_state->assets, "scripts/main", nullptr);
	if (main_py == nullptr)
	{
		return fatal_error(nullptr, "Script engine failed");
	}

	if (!script_engine_exec(main_py, "main.py"))
	{
		SDL_free(main_py);
		return fatal_error(nullptr, "Script failed");
	}
	SDL_free(main_py);

	constexpr SDL_InitFlags init_flags =
		SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_HAPTIC
		| SDL_INIT_GAMEPAD
		| SDL_INIT_EVENTS;

	if (!SDL_Init(init_flags))
	{
		return fatal_error(nullptr, "Initialisation failed");
	}

	return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppIterate([[maybe_unused]] void *appstate)
{
	return SDL_APP_SUCCESS;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_Event *event)
{
	return SDL_APP_SUCCESS;
}

void SDL_AppQuit([[maybe_unused]] void *appstate, [[maybe_unused]] SDL_AppResult result)
{
	script_engine_destroy();

	const app_state_t *app_state = appstate;
	if (app_state->assets != nullptr)
	{
		assets_destroy(app_state->assets);
	}

	SDL_free(appstate);
}
