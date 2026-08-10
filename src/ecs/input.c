#include "chirp/input.h"
#include "ecs.h"
#include "ecs/components.h"

#include "flecs.h"

#include <SDL3/SDL_error.h>

static void create_input([[maybe_unused]] ecs_iter_t *iter)
{
	input_t input;
	if (!input_create(&input))
	{
		ecs_set_error("Input error", SDL_GetError());
		return;
	}

	ecs_set_id(ecs_world(), ecs_singleton(EcsInput),
		sizeof(input_t), &input);
}

void ecs_add_input()
{
	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsInit), .inout = EcsInOutNone},
		},
		.events = {EcsOnSet},
		.callback = create_input,
	});
}
