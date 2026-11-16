#pragma once

typedef struct
{
	const char *name;
	const char *version;
	const char *identifier;
	const char *build_type;
	const char *compiler;
	const bool debug;
} engine_config_t;

[[nodiscard]]
engine_config_t engine_config_get();
