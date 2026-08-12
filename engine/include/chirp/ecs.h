#pragma once

#include "flecs.h"

typedef struct
{
	const char *name;
	const char *version;
	const char *identifier;
	const char *creator;
	const char *copyright;
	const char *url;
	const char *type;
} metadata_t;

/** Get/set data in singleton */
#define ecs_singleton(component)	\
	component, component

/** Filter by singleton */
#define ecs_singleton_id(component)	\
	component, .src.id = component

#define ecs_scope(world, scope)											\
	for (const ecs_entity_t prev_scope = ecs_set_scope(world, scope);	\
		ecs_get_scope(world) == (scope);								\
		ecs_set_scope(world, prev_scope))

void ecs_create_default();

void ecs_destroy_default();

[[nodiscard]]
ecs_world_t *ecs_world();

void ecs_add_assets();
