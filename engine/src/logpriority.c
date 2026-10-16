#include "chirp/logpriority.h"
#include "chirp/logcategory.h"

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_error.h>

[[nodiscard]]
static SDL_LogPriority parse_log_priority(const char *name)
{
	// There are technically more categories,
	// but these are the "main ones" I feel

	if (SDL_strcmp(name, "debug") == 0)
	{
		return SDL_LOG_PRIORITY_DEBUG;
	}

	if (SDL_strcmp(name, "info") == 0)
	{
		return SDL_LOG_PRIORITY_INFO;
	}

	if (SDL_strcmp(name, "warn") == 0)
	{
		return SDL_LOG_PRIORITY_WARN;
	}

	if (SDL_strcmp(name, "error") == 0)
	{
		return SDL_LOG_PRIORITY_ERROR;
	}

	return SDL_LOG_PRIORITY_INVALID;
}

bool log_priority_set_from_string(const char *str)
{
	char *priorities = SDL_strdup(str);

	char *current = priorities;
	while (current != nullptr)
	{
		char *end = SDL_strchr(current, ',');
		if (end != nullptr)
		{
			*end = '\0';
		}

		char *sep = SDL_strchr(current, '=');
		if (sep == nullptr) // Global log priority
		{
			const SDL_LogPriority log_priority = parse_log_priority(current);
			if (log_priority == SDL_LOG_PRIORITY_INVALID)
			{
				SDL_SetError("Unknown priority: '%s'", current);
				SDL_free(priorities);
				return false;
			}

			SDL_SetLogPriorities(log_priority);
		}
		else // Category log priority
		{
			*sep = '\0';

			const int category = log_category_parse(current);
			if (category < 0)
			{
				SDL_SetError("Unknown category: '%s'", current);
				SDL_free(priorities);
				return false;
			}

			const SDL_LogPriority log_priority = parse_log_priority(sep + 1);
			if (log_priority == SDL_LOG_PRIORITY_INVALID)
			{
				SDL_SetError("Unknown priority: '%s'", sep + 1);
				SDL_free(priorities);
				return false;
			}

			SDL_SetLogPriority(category, log_priority);
		}

		current = end != nullptr
			? end + 1
			: nullptr;
	}

	SDL_free(priorities);
	return true;
}
