#pragma once

#include "chirp/vector.h"

#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

constexpr size_t ui_combo_max_items = 16;
constexpr size_t ui_edit_string_max_length = 64;

typedef struct
{
	float height;        // Fixed row height for all items
	Sint32 columns;      // Number of columns
	const float *ratio;  // Enable dynamic mode and set ratio for each column(0-1)
	const float *widths; // Enable fixed mode and set width for each column
} ui_row_t;

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
	char *text;
	ui_text_align_t align;
} ui_label_t;

typedef struct
{
	const char *items[ui_combo_max_items];
	Sint32 count;
	Sint32 selected;
	Sint32 item_height;
	vector2f_t size;
} ui_combo_t;

typedef struct
{
	char buffer[ui_edit_string_max_length];
	Sint32 length;
} ui_edit_string_t;
