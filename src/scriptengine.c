#include "chirp/scriptengine.h"
#include "chirp/logcategory.h"

#include <pocketpy/pocketpy.h>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>

static void pk_write_fn(const char *text)
{
	SDL_LogInfo(LOG_CATEGORY_SCRIPT, "%s", text);
}

void script_engine_create()
{
	py_initialize();

	py_Callbacks *callbacks = py_callbacks();
	callbacks->print = &pk_write_fn;
}

void script_engine_destroy()
{
	py_finalize();
}

bool script_engine_exec(const char *source, const char *filename)
{
	if (py_exec(source, filename, EXEC_MODE, nullptr))
	{
		return true;
	}

	return SDL_SetError("%s", py_formatexc());
}
