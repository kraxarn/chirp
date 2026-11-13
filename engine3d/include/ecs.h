#pragma once

#include "chirp/matrix.h"

#include <SDL3/SDL_stdinc.h>

typedef struct
{
	bool rebuild;
	matrix4x4_t value;
} projection_t;

typedef struct
{
	char *name;
} model_descriptor_t;

typedef struct
{
	float height;        // Fixed row height for all items
	int columns;         // Number of columns
	const float *ratio;  // Enable dynamic mode and set ratio for each column(0-1)
	const float *widths; // Enable fixed mode and set width for each column
} ui_row_def_t;

void ecs_create();

void ecs_destroy();

#define ecs_phase(phase)	\
	ecs_dependson(EcsPhase##phase) // NOLINT(bugprone-signed-bitwise)

void ecs_add_nkui();
void ecs_add_ui();
void ecs_add_window();
void ecs_add_gpu();
void ecs_add_physics();
void ecs_add_render();
void ecs_add_models();
void ecs_add_logging();
