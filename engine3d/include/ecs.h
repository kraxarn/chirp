#pragma once

#include "chirp/matrix.h"

#include <SDL3/SDL_stdinc.h>

// TODO: Maybe just include flecs here directly?
typedef struct ecs_world_t ecs_world_t;
typedef struct ecs_iter_t ecs_iter_t;
typedef Uint64 ecs_id_t;
typedef Uint64 ecs_entity_t;

typedef Sint32 py_vm_index_t;
typedef matrix4x4_t world_transform_t;

typedef struct
{
	bool rebuild;
	matrix4x4_t value;
} projection_t;

typedef struct
{
	char *name;
} model_descriptor_t;

void ecs_create();

void ecs_destroy();

#define ecs_phase(phase)	\
	ecs_dependson(EcsPhase##phase) // NOLINT(bugprone-signed-bitwise)

#define ecs_observer_init_all(o)					\
	for (size_t i = 0; i < SDL_arraysize(o); i++)	\
		ecs_observer_init(ecs_world(), o + i);

void ecs_add_nkui();
void ecs_add_window();
void ecs_add_gpu();
void ecs_add_physics();
void ecs_add_render();
void ecs_add_models();
void ecs_add_logging();
