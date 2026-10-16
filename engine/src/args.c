#include "chirp/args.h"
#include "chirp/map.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

typedef enum : Uint8
{
	TYPE_NONE,  // --arg
	TYPE_FLAG,  // --arg / --no-arg
	TYPE_VALUE, // --arg [value]
} arg_type_t;

typedef struct
{
	arg_type_t type;
	char *description;
	SDL_PropertyType value_type;
	void *value;
} arg_command_t;

typedef enum : Uint8
{
	RESULT_INVALID,      // No result set
	RESULT_OK,           // Success
	RESULT_NOT_ARGUMENT, // Invalid argument
	RESULT_NOT_FOUND,    // Unknown argument
} args_parse_result_t;

typedef struct
{
	const char *arg;
	const char *value;
	args_parse_result_t result;
} args_parse_context_t;

args_t args_create()
{
	return map_create();
}

void args_destroy(const args_t args)
{
	SDL_DestroyProperties(args);
}

static void parse(void *userdata, const SDL_PropertiesID props, const char *name)
{
	args_parse_context_t *ctx = userdata;
	if (ctx->result == RESULT_OK)
	{
		return;
	}

	if (SDL_strncasecmp(ctx->arg, "--", 2) != 0)
	{
		ctx->result = RESULT_NOT_ARGUMENT;
		return;
	}

	const arg_command_t *cmd = SDL_GetPointerProperty(props, name, nullptr);
	SDL_assert(cmd != nullptr);

	if (cmd->type == TYPE_NONE
		&& SDL_strcasecmp(ctx->arg + 2, name) == 0)
	{
		*((bool*) cmd->value) = true;
		ctx->result = RESULT_OK;
		return;
	}

	if (cmd->type == TYPE_FLAG)
	{
		if (SDL_strncasecmp(ctx->arg, "--no-", 5) == 0
			&& SDL_strcasecmp(ctx->arg + 5, name) == 0)
		{
			*((bool*) cmd->value) = false;
			ctx->result = RESULT_OK;
			return;
		}

		if (SDL_strcasecmp(ctx->arg + 2, name) == 0)
		{
			*((bool*) cmd->value) = true;
			ctx->result = RESULT_OK;
			return;
		}
	}

	if (cmd->type == TYPE_VALUE
		&& SDL_strcasecmp(ctx->arg + 2, name) == 0
		&& ctx->value != nullptr)
	{
		if (cmd->value_type == SDL_PROPERTY_TYPE_STRING)
		{
			// ctx->value should be set from argv, so it should always be available
			*((const char**) cmd->value) = ctx->value;
			ctx->result = RESULT_OK;
			return;
		}

		if (cmd->value_type == SDL_PROPERTY_TYPE_NUMBER)
		{
			*((Sint64*) cmd->value) = SDL_strtoll(ctx->value, nullptr, 10);
			ctx->result = RESULT_OK;
			return;
		}
	}

	ctx->result = RESULT_NOT_FOUND;
}

bool args_parse(const args_t args, const int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		args_parse_context_t context = {
			.arg = argv[i],
			// TODO: If we use value, we should increment i
			.value = i + 1 < argc ? argv[i + 1] : nullptr,
			.result = RESULT_INVALID,
		};

		if (!SDL_EnumerateProperties(args, parse, &context))
		{
			return false;
		}

		if (context.result == RESULT_NOT_FOUND)
		{
			return SDL_SetError("Unknown arg: '%s'", argv[i]);
		}
	}

	return true;
}

static void print(const char *message)
{
	const SDL_LogOutputFunction log_func = SDL_GetDefaultLogOutputFunction();
	log_func(nullptr, SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, message);
}

static void name_length(void *userdata, [[maybe_unused]] const SDL_PropertiesID props, const char *name)
{
	int *length = userdata;
	*length = SDL_max(*length, SDL_strlen(name));
}

static void print_command(void *userdata, const SDL_PropertiesID props, const char *name)
{
	constexpr size_t buffer_len = 256;
	char buffer[buffer_len];

	const int length = *(int*) userdata;

	const arg_command_t *cmd = SDL_GetPointerProperty(props, name, nullptr);
	SDL_assert(cmd != nullptr);

	SDL_snprintf(buffer, buffer_len, "  %s%-*s %s",
		cmd->type == TYPE_FLAG ? "--(no-)" : "--",
		length + (cmd->type == TYPE_FLAG ? 0 : 5),
		name, cmd->description
	);

	print(buffer);
}

bool args_print_help(const args_t args, const char *name)
{
	// To avoid custom log formatting
	// (no need to clean-up since we should exit immediately afterwards)
	SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, "");

	constexpr size_t buffer_len = 64;
	char buffer[buffer_len];
	SDL_snprintf(buffer, buffer_len, "Usage: %s [options]", name);
	print(buffer);

	int length = 0;
	return (bool) (SDL_EnumerateProperties(args, name_length, &length)
		&& SDL_EnumerateProperties(args, print_command, &length));
}

static void cleanup_command([[maybe_unused]] void *userdata, void *value)
{
	arg_command_t *command = value;

	SDL_free(command->description);
	SDL_free(command);
}

static bool add_command(const args_t args, const char *name,
	const arg_type_t command_type, const SDL_PropertyType value_type,
	const char *description, void *value)
{
	arg_command_t *command = SDL_malloc(sizeof(arg_command_t));
	command->type = command_type;
	command->description = SDL_strdup(description);
	command->value_type = value_type;
	command->value = value;

	if (!SDL_SetPointerPropertyWithCleanup(args, name,
		command, cleanup_command, nullptr))
	{
		cleanup_command(nullptr, command);
		return false;
	}

	return true;
}

bool args_add(const args_t args, const char *name,
	const char *description, bool *value)
{
	return add_command(args, name, TYPE_NONE,
		SDL_PROPERTY_TYPE_BOOLEAN, description, value);
}

bool args_add_flag(const args_t args, const char *name,
	const char *description, bool *value)
{
	return add_command(args, name, TYPE_FLAG,
		SDL_PROPERTY_TYPE_BOOLEAN, description, value);
}

bool args_add_string(const args_t args, const char *name,
	const char *description, char **value)
{
	return add_command(args, name, TYPE_VALUE,
		SDL_PROPERTY_TYPE_STRING, description, value);
}

bool args_add_int(const args_t args, const char *name,
	const char *description, Sint64 *value)
{
	return add_command(args, name, TYPE_VALUE,
		SDL_PROPERTY_TYPE_NUMBER, description, value);
}
