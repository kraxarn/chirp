#pragma once

#include "chirp/matrix.h"
#include "chirp/vector.h"

#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

constexpr size_t ui_label_max_length = 256;
constexpr size_t ui_edit_string_max_length = 64;

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

typedef enum : Uint32 // To match nk_flags
{
	UI_TEXT_ALIGN_LEFT     = 1U << 0U,
	UI_TEXT_ALIGN_CENTERED = 1U << 1U,
	UI_TEXT_ALIGN_RIGHT    = 1U << 2U,
	UI_TEXT_ALIGN_TOP      = 1U << 3U,
	UI_TEXT_ALIGN_MIDDLE   = 1U << 4U,
	UI_TEXT_ALIGN_BOTTOM   = 1U << 5U,

	UI_TEXT_ALIGN_MIDDLE_LEFT     = UI_TEXT_ALIGN_MIDDLE | UI_TEXT_ALIGN_LEFT,
	UI_TEXT_ALIGN_MIDDLE_CENTERED = UI_TEXT_ALIGN_MIDDLE | UI_TEXT_ALIGN_CENTERED,
	UI_TEXT_ALIGN_MIDDLE_RIGHT    = UI_TEXT_ALIGN_MIDDLE | UI_TEXT_ALIGN_RIGHT,
} ui_text_align_t; // nk_text_align / nk_text_alignment

typedef struct
{
	char text[ui_label_max_length];
	ui_text_align_t align;
} ui_label_t;

typedef struct
{
	const char **items;
	int count;
	int selected;
	int item_height;
	vector2f_t size;
} ui_combo_t;

typedef struct
{
	char buffer[ui_edit_string_max_length];
	int length;
} ui_edit_string_t;

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
