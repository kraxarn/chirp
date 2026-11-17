#include "ecs/ui.h"
#include "ecs.h"
#include "nkui.h"
#include "ecs/components.h"
#include "ecs/entities.h"
#include "ecs/tags.h"

#include "flecs.h"
#include "chirp/ecs.h"
#include "chirp/logcategory.h"
#include "chirp/ecs/modules.h"
#include "flecs/addons/flecs_c.h"
#include "flecs/addons/system.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include <stddef.h>

static bool window_begin(nk_context_t *ctx, const ecs_world_t *stage,
	const ecs_entity_t entity, const char *title, const nk_rect_t bounds)
{
#define flag(component, flag) (ecs_has_id(stage, entity, (component)) ? (nk_flags) (flag) : 0)
	const nk_flags flags = (title != nullptr && title[0] != '\0' ? (nk_flags) NK_WINDOW_TITLE : 0)
		| flag(EcsUiBorder, NK_WINDOW_BORDER)
		| flag(EcsUiMovable, NK_WINDOW_MOVABLE)
		| flag(EcsUiScalable, NK_WINDOW_SCALABLE)
		| flag(EcsUiClosable, NK_WINDOW_CLOSABLE)
		| flag(EcsUiMinimizable, NK_WINDOW_MINIMIZABLE)
		| flag(EcsUiNoScrollbar, NK_WINDOW_NO_SCROLLBAR)
		| flag(EcsUiScrollAutoHide, NK_WINDOW_SCROLL_AUTO_HIDE);
#undef flag

	// TODO: Fallback to entity ID
	const char *name = ecs_get_name(stage, entity);

	return nk_begin_titled(ctx, name, title, bounds, flags);
}

static void window_end(nk_context_t *ctx)
{
	nk_end(ctx);
}

static void layout_row(nk_context_t *ctx, const ui_row_t *row)
{
	SDL_assert(row->ratio == nullptr || row->widths == nullptr);

	if (row->ratio != nullptr)
	{
		nk_layout_row(ctx, NK_DYNAMIC, row->height, row->columns, row->ratio);
	}
	else if (row->widths != nullptr)
	{
		nk_layout_row(ctx, NK_STATIC, row->height, row->columns, row->widths);
	}
	else
	{
		nk_layout_row_dynamic(ctx, row->height, row->columns);
	}
}

static void ui_combo(nk_context_t *ctx, ui_combo_t *combo)
{
	combo->selected = nk_combo(ctx, combo->items, combo->count, combo->selected,
		combo->item_height, nk_vec2(combo->size.x, combo->size.y));
}

static void ui_edit_string(nk_context_t *ctx, SDL_Window *window, ui_edit_string_t *edit)
{
	const nk_edit_events_t events = nk_edit_string(ctx, NK_EDIT_FIELD, edit->buffer,
		&edit->length, ui_edit_string_max_length, nk_filter_default);

	if ((events & (nk_flags) NK_EDIT_ACTIVATED) > 0
		&& !SDL_StartTextInput(window))
	{
		SDL_LogError(LOG_CATEGORY_UI, "Failed to start text input: %s",
			SDL_GetError());
	}

	if ((events & (nk_flags) NK_EDIT_DEACTIVATED) > 0
		&& !SDL_StopTextInput(window))
	{
		SDL_LogError(LOG_CATEGORY_UI, "Failed to stop text input: %s",
			SDL_GetError());
	}
}

static void draw_element(nk_context_t *ctx, const ecs_world_t *stage,
	SDL_Window *window, const ecs_entity_t entity)
{
	const ui_row_t *row = ecs_get_id(stage, entity, EcsUiRow);
	if (row != nullptr)
	{
		layout_row(ctx, row);
		return;
	}

	const ui_label_t *label = ecs_get_id(stage, entity, EcsUiLabel);
	if (label != nullptr)
	{
		nk_label(ctx, label->text, label->align);
		return;
	}

	ui_combo_t *combo = ecs_get_mut_id(stage, entity, EcsUiCombo);
	if (combo != nullptr)
	{
		ui_combo(ctx, combo);
		return;
	}

	ui_edit_string_t *edit_string = ecs_get_mut_id(stage, entity, EcsUiEditString);
	if (edit_string != nullptr)
	{
		ui_edit_string(ctx, window, edit_string);
		return;
	}
}

static void prepare_ui(ecs_iter_t *iter)
{
	nk_context_t *ctx = &ecs_field(iter, nkui_context_t, 0)->nk;
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 1);
	const nk_rect_t *w_bounds = ecs_field(iter, nk_rect_t, 2);
	const char **w_titles = ecs_field(iter, const char*, 3);

	for (Sint32 i = 0; i < iter->count; i++)
	{
		const ecs_entity_t parent = iter->entities[i];

		// TODO: Default to title with entity ID
		const char *title = w_titles != nullptr ? w_titles[i] : "";

		SDL_assert(w_bounds != nullptr);
		const nk_rect_t bounds = w_bounds[i];

		if (window_begin(ctx, iter->stage, parent, title, bounds))
		{
			constexpr size_t queue_size = 32;
			ecs_entity_t queue[queue_size];
			size_t queue_current = 0;
			size_t queue_next = 1;

			SDL_zeroa(queue);
			queue[queue_current] = parent;

			while (queue[queue_current] != 0)
			{
				const ecs_entity_t entity = queue[queue_current];
				queue[queue_current] = 0;

				draw_element(ctx, iter->stage, window, entity);

				ecs_iter_t children = ecs_children(iter->stage, entity);
				while (ecs_children_next(&children))
				{
					for (Sint32 cc = 0; cc < children.count; cc++)
					{
						SDL_assert(queue[queue_next] == 0);
						queue[queue_next] = children.entities[cc];
						queue_next = (queue_next + 1) % queue_size;
					}
				}
				ecs_iter_fini(&children);

				queue_current = (queue_current + 1) % queue_size;
			}
		}
		window_end(ctx);
	}
}

static void validate_row(ecs_iter_t *iter)
{
	const ui_row_t *row = ecs_field(iter, ui_row_t, 0);
	if (row->ratio == nullptr || row->widths == nullptr)
	{
		return;
	}

	SDL_LogWarn(LOG_CATEGORY_UI,
		"Can't set both dynamic ratio and fixed widths at the same time"
	);
}

void ecs_add_ui()
{
	ecs_scope(ecs_world(), EcsChirpSystem)
	{
		const ecs_entity_t entity_prepare_ui = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "PrepareUi",
		});
		ecs_add_id(ecs_world(), entity_prepare_ui, ecs_phase(Update));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_prepare_ui,
			.query.terms = {
				// TODO: Maybe bake all window tags into the query?
				(ecs_term_t){.id = ecs_singleton_id(EcsNkContext), .inout = EcsInOut},
				(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
				(ecs_term_t){.id = EcsUiWindow, .inout = EcsIn},
				(ecs_term_t){.id = EcsUiTitle, .oper = EcsOptional, .inout = EcsIn},
			},
			.callback = prepare_ui,
		});
	}

	ecs_scope(ecs_world(), EcsChirpObserver)
	{
		ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
			.entity = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
				.name = "ValidateUiRowDef",
			}),
			.query.terms = {
				(ecs_term_t){.id = EcsUiRow, .inout = EcsIn},
			},
			.events = {EcsOnSet},
			.callback = validate_row,
		});
	}
}
