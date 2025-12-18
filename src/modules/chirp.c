#include "chirp/modules.h"

#define PK_IS_PUBLIC_INCLUDE
#include "pocketpy/pocketpy.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

typedef struct game_t
{
	SDL_Window *window;
	SDL_Renderer *renderer;
} game_t;

static bool game_new(const int argc, py_TValue *argv)
{
	PY_CHECK_ARGC(1);

	const py_Type type = py_totype(argv);
	py_newobject(py_retval(), type, 0, sizeof(game_t));
	return true;
}

static bool create_window(SDL_Window **window, const char *title)
{
	constexpr auto width = 1280;
	constexpr auto height = 720;
	constexpr auto flags = SDL_WINDOW_RESIZABLE;

	*window = SDL_CreateWindow(title, width, height, flags);
	return *window != nullptr;
}

static bool create_renderer(SDL_Window *window, SDL_Renderer **renderer)
{
	*renderer = SDL_CreateRenderer(window, nullptr);
	return *renderer != nullptr;
}

static bool game_init(const int argc, py_TValue *argv)
{
	PY_CHECK_ARGC(5);
	PY_CHECK_ARG_TYPE(1, tp_str);
	PY_CHECK_ARG_TYPE(2, tp_str);
	PY_CHECK_ARG_TYPE(3, tp_str);
	PY_CHECK_ARG_TYPE(4, tp_str);

	game_t *game = py_touserdata(py_arg(0));

	const char *name = py_tostr(py_arg(1));
	const char *version = py_tostr(py_arg(2));
	const char *identifier = py_tostr(py_arg(3));
	const char *creator = py_tostr(py_arg(4));

	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, name);
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, version);
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, identifier);
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, creator);
	// TODO: This is technically an argument, only defaulting to "game", but maybe we want to force it?
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

	constexpr SDL_InitFlags init_flags =
		SDL_INIT_AUDIO
		| SDL_INIT_VIDEO
		| SDL_INIT_HAPTIC
		| SDL_INIT_GAMEPAD
		| SDL_INIT_EVENTS;

	if (!SDL_Init(init_flags)
		|| !create_window(&game->window, name)
		|| !create_renderer(game->window, &game->renderer))
	{
		return py_exception(tp_RuntimeError, "%s", SDL_GetError());
	}

	char *title = nullptr;
	SDL_asprintf(&title, "%s %s (%s)", name, version, SDL_GetRendererName(game->renderer));
	SDL_SetWindowTitle(game->window, title);
	SDL_free(title);

	py_newnone(py_retval());
	return true;
}

static void game_dtor(void *data)
{
	const game_t *game = data;
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
}

void add_module_chirp()
{
	py_TValue *mod = py_newmodule("chirp");

	const py_Type game = py_newtype("Game", tp_object, mod, game_dtor);
	py_bindmethod(game, "__new__", game_new);
	py_bindmethod(game, "__init__", game_init);
}
