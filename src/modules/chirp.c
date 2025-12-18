#include "chirp/modules.h"

#define PK_IS_PUBLIC_INCLUDE
#include "pocketpy/pocketpy.h"

#include <SDL3/SDL_init.h>

typedef struct game_t
{
} game_t;

static bool game_new([[maybe_unused]] int argc, py_TValue *argv)
{
	const py_Type type = py_totype(argv);
	py_newobject(py_retval(), type, 0, sizeof(game_t));
	return true;
}

static bool game_init(int argc, py_TValue *argv)
{
	PY_CHECK_ARGC(5);
	PY_CHECK_ARG_TYPE(1, tp_str);
	PY_CHECK_ARG_TYPE(2, tp_str);
	PY_CHECK_ARG_TYPE(3, tp_str);
	PY_CHECK_ARG_TYPE(4, tp_str);

	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, py_tostr(py_arg(1)));
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, py_tostr(py_arg(2)));
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, py_tostr(py_arg(3)));
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, py_tostr(py_arg(4)));
	// TODO: This is technically an argument, only defaulting to "game", but maybe we want to force it?
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

	py_newnone(py_retval());
	return true;
}

static void game_dtor(void *data)
{
}

void add_module_chirp()
{
	py_TValue *mod = py_newmodule("chirp");

	const py_Type game = py_newtype("Game", tp_object, mod, game_dtor);
	py_bindmethod(game, "__new__", game_new);
	py_bindmethod(game, "__init__", game_init);
}
