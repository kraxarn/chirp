#include "game.h"
#include "ecs.h"
#include "ecs/components.h"
#include "ui/debugoverlay.h"

#include "flecs.h"
#include "chirp/ecs.h"
#include "chirp/ecs/components.h"

// TODO: Temporary, shouldn't be in engine logic

void game_init(ecs_world_t *world)
{
	const ecs_entity_t entity_draw_ui = ecs_entity_init(world, &(ecs_entity_desc_t){
		.name = "DrawUi",
	});
	ecs_add_id(world, entity_draw_ui,
		ecs_dependson(ecs_phase(PHASE_UPDATE)));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_draw_ui,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsNkContext), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsIn},
			(ecs_term_t){.id = EcsPhysicsBody, .src.name = "$plr_bdy", .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsTimeStats), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsInput), .inout = EcsIn},
			(ecs_term_t){
				.first.id = EcsPredEq,
				.src.name = "$plr_bdy",
				.second = (ecs_term_ref_t){.id = EcsIsName, .name = "Player"},
				.inout = EcsInOutNone,
			},
		},
		.callback = draw_debug_overlay,
	});
}
