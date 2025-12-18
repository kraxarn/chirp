#include "chirp/assets.h"

#include "chirp/logcategory.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

typedef enum assets_type_t: Uint8
{
	TYPE_NONE      = 0,
	TYPE_FILE      = 1,
	TYPE_DIRECTORY = 2,
} assets_type_t;

typedef struct assets_t
{
	assets_type_t type;
	char *path;
} assets_t;

static bool create_from_file([[maybe_unused]] assets_t *assets)
{
	const char *base_path = SDL_GetBasePath();
	if (base_path == nullptr)
	{
		return false;
	}

	char *path = nullptr;
	if (SDL_asprintf(&path, "%sassets.nest", base_path) < 0)
	{
		return false;
	}

	SDL_PathInfo info;
	if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_FILE)
	{
		return false;
	}

	SDL_LogVerbose(LOG_CATEGORY_ASSETS, "Loading assets from file: %s", path);

	assets->type = TYPE_FILE;
	assets->path = path;

	// TODO
	return SDL_SetError("Loading from file not supported");
}

static bool create_from_directory([[maybe_unused]] assets_t *assets)
{
	char *cwd = SDL_GetCurrentDirectory();
	if (cwd == nullptr)
	{
		return false;
	}

	char *path = nullptr;
	if (SDL_asprintf(&path, "%sassets", cwd) < 0)
	{
		SDL_free(cwd);
		return false;
	}
	SDL_free(cwd);

	SDL_PathInfo info;
	if (!SDL_GetPathInfo(path, &info) || info.type != SDL_PATHTYPE_DIRECTORY)
	{
		return false;
	}

	SDL_LogVerbose(LOG_CATEGORY_ASSETS, "Loading assets from directory: %s", path);

	assets->type = TYPE_DIRECTORY;
	assets->path = path;

	return true;
}

assets_t *assets_create()
{
	assets_t *assets = SDL_malloc(sizeof(assets_t));
	if (assets == nullptr)
	{
		return nullptr;
	}

	if ((int) create_from_file(assets))
	{
		return assets;
	}
	SDL_LogWarn(LOG_CATEGORY_ASSETS, "Failed to load assets from file: %s", SDL_GetError());

	if ((int) create_from_directory(assets))
	{
		return assets;
	}
	SDL_LogWarn(LOG_CATEGORY_ASSETS, "Failed to load assets from directory: %s", SDL_GetError());

	SDL_free(assets);
	SDL_SetError("Assets not found or invalid");
	return nullptr;
}

void assets_destroy(assets_t *assets)
{
	SDL_free(assets->path);
	SDL_free(assets);
}

void *assets_load(const assets_t *assets, const char *filename, size_t *size)
{
	if (assets->type == TYPE_FILE)
	{
		SDL_SetError("Not supported"); // TODO
		return nullptr;
	}

	if (assets->type == TYPE_DIRECTORY)
	{
		char *pattern;
		SDL_asprintf(&pattern, "%s.*", filename);

		auto count = 0;
		char **matches = SDL_GlobDirectory(assets->path, pattern, 0, &count);

		if (count == 0)
		{
			SDL_free(pattern);
			SDL_free((void *) matches);
			SDL_SetError("File not found");
			return nullptr;
		}

		if (count > 1)
		{
			SDL_free(pattern);
			SDL_free((void *) matches);
			SDL_SetError("More than one file found");
			return nullptr;
		}

		SDL_LogVerbose(LOG_CATEGORY_ASSETS, "Resolved asset '%s' from '%s'", matches[0], filename);

		void *data = SDL_LoadFile(matches[0], size);

		SDL_free(pattern);
		SDL_free((void *) matches);
		return data;
	}

	return nullptr;
}
