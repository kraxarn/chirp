#include "ecs.h"
#include "nkui.h"
#include "ecs/components.h"
#include "ecs/entities.h"
#include "ecs/tags.h"

#include "flecs.h"
#include "chirp/ecs.h"
#include "chirp/ecs/modules.h"
#include "flecs/addons/flecs_c.h"
#include "flecs/addons/system.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

static void draw_ui(ecs_iter_t *iter)
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

#define flag(component, flag) (ecs_has_id(iter->stage, window, (component)) ? (nk_flags) (flag) : 0)
		const nk_flags flags = (title != nullptr ? (nk_flags) NK_WINDOW_TITLE : 0)
			| flag(EcsUiBorder, NK_WINDOW_BORDER)
			| flag(EcsUiMovable, NK_WINDOW_MOVABLE)
			| flag(EcsUiScalable, NK_WINDOW_SCALABLE)
			| flag(EcsUiClosable, NK_WINDOW_CLOSABLE)
			| flag(EcsUiMinimizable, NK_WINDOW_MINIMIZABLE)
			| flag(EcsUiNoScrollbar, NK_WINDOW_NO_SCROLLBAR)
			| flag(EcsUiScrollAutoHide, NK_WINDOW_SCROLL_AUTO_HIDE);
#undef flag

		if (nk_begin(ctx, title, bounds, flags))
		{
		}
		nk_end(ctx);
	}
}

void ecs_add_ui()
{
	ecs_scope(ecs_world(), EcsChirpSystem)
	{
		const ecs_entity_t entity_draw_ui = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "DrawUi",
		});
		ecs_add_id(ecs_world(), entity_draw_ui, ecs_phase(Update));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_draw_ui,
			.query.terms = {
				// TODO: Maybe bake all window tags into the query?
				(ecs_term_t){.id = ecs_singleton_id(EcsNkContext), .inout = EcsIn},
				(ecs_term_t){.id = EcsUiWindow, .inout = EcsIn},
				(ecs_term_t){.id = EcsUiTitle, .oper = EcsOptional, .inout = EcsIn},
			},
			.callback = draw_ui,
		});
	}
}
