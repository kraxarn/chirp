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
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

static bool window_begin(nk_context_t *ctx, const ecs_world_t *stage,
	const ecs_entity_t entity, const char *title, const nk_rect_t bounds)
{
#define flag(component, flag) (ecs_has_id(stage, entity, (component)) ? (nk_flags) (flag) : 0)
	const nk_flags flags = (title != nullptr ? (nk_flags) NK_WINDOW_TITLE : 0)
		| flag(EcsUiBorder, NK_WINDOW_BORDER)
		| flag(EcsUiMovable, NK_WINDOW_MOVABLE)
		| flag(EcsUiScalable, NK_WINDOW_SCALABLE)
		| flag(EcsUiClosable, NK_WINDOW_CLOSABLE)
		| flag(EcsUiMinimizable, NK_WINDOW_MINIMIZABLE)
		| flag(EcsUiNoScrollbar, NK_WINDOW_NO_SCROLLBAR)
		| flag(EcsUiScrollAutoHide, NK_WINDOW_SCROLL_AUTO_HIDE);
#undef flag

	return nk_begin(ctx, title, bounds, flags);
}

static void window_end(nk_context_t *ctx)
{
	nk_end(ctx);
}

static void layout_row(nk_context_t *ctx, const ui_row_def_t def)
{
	SDL_assert(def.ratio == nullptr || def.widths == nullptr);

	if (def.ratio != nullptr)
	{
		nk_layout_row(ctx, NK_DYNAMIC, def.height, def.columns, def.ratio);
	}
	else if (def.widths != nullptr)
	{
		nk_layout_row(ctx, NK_STATIC, def.height, def.columns, def.widths);
	}
	else
	{
		nk_layout_row_dynamic(ctx, def.height, def.columns);
	}
}

static void prepare_ui(ecs_iter_t *iter)
{
	nk_context_t *ctx = &ecs_field(iter, nkui_context_t, 0)->nk;
	const nk_rect_t *w_bounds = ecs_field(iter, nk_rect_t, 1);
	const char **w_titles = ecs_field(iter, const char*, 2);

	for (Sint32 i = 0; i < iter->count; i++)
	{
		const ecs_entity_t window = iter->entities[i];

		// TODO: Default to title with entity ID
		const char *title = w_titles != nullptr ? w_titles[i] : "";

		SDL_assert(w_bounds != nullptr);
		const nk_rect_t bounds = w_bounds[i];

		if (window_begin(ctx, iter->stage, window, title, bounds))
		{
			constexpr size_t queue_size = 32;
			ecs_entity_t queue[queue_size];
			size_t queue_current = 0;
			size_t queue_next = 1;

			SDL_zeroa(queue);
			queue[queue_current] = window;

			while (queue[queue_current] != 0)
			{
				const ecs_entity_t entity = queue[queue_current];
				queue[queue_current] = 0;

				const ui_row_def_t *row_def = ecs_get_id(iter->stage, entity, EcsUiRow);
				if (row_def != nullptr)
				{
					layout_row(ctx, *row_def);
				}

				const ui_label_t *label = ecs_get_id(iter->stage, entity, EcsUiLabel);
				if (label != nullptr)
				{
					nk_label(ctx, label->text, label->align);
				}

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
	const ui_row_def_t def = *ecs_field(iter, ui_row_def_t, 0);
	if (def.ratio == nullptr || def.widths == nullptr)
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
		ecs_add_id(ecs_world(), entity_prepare_ui, ecs_phase(UpdateEnd));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_prepare_ui,
			.query.terms = {
				// TODO: Maybe bake all window tags into the query?
				(ecs_term_t){.id = ecs_singleton_id(EcsNkContext), .inout = EcsIn},
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
