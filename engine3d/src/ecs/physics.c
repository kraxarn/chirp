#include "args.h"
#include "ecs.h"
#include "timestats.h"
#include "ecs/components.h"

#include "chirp/physics.h"
#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/id.h"
#include "chirp/ecs.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"
#include "ecs/entities.h"
#include "flecs/addons/flecs_c.h"

#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>

static void init_physics([[maybe_unused]] ecs_iter_t *iter)
{
	// TODO: Currently broken
#ifdef SDL_PLATFORM_EMSCRIPTEN
	const Uint32 workers = 1;
#else
	const arg_values_t *args = ecs_field(iter, arg_values_t, 0);
	const Uint32 workers = args->physics_threads > 0
		? args->physics_threads
		: SDL_GetNumLogicalCPUCores();
#endif

	b3WorldId world = {0};
	if (!physics_create(workers, &world))
	{
		ecs_set_error("Physics error", SDL_GetError());
		return;
	}

	ecs_set_id(ecs_world(), ecs_singleton(EcsPhysicsWorld),
		sizeof(b3WorldId), &world);
}

static void update_physics(ecs_iter_t *iter)
{
	const b3WorldId world = *ecs_field(iter, b3WorldId, 0);
	const time_stats_t *time_stats = ecs_field(iter, time_stats_t, 1);

	b3World_Step(world, iter->delta_time, physics_sub_step_count(time_stats->fps));
}

static void sync_physics(ecs_iter_t *iter)
{
	const b3BodyId *body_ids = ecs_field(iter, b3BodyId, 0);
	vector3f_t *positions = ecs_field(iter, vector3f_t, 1);
	projection_t *projections = ecs_field(iter, projection_t, 2);

	for (Sint32 i = 0; i < iter->count; i++)
	{
		const b3Pos b3_pos = b3Body_GetPosition(body_ids[i]);
		positions[i] = vector3f_from_b3vec3(b3_pos);

		// TODO: Fix rotation

		projections[i].rebuild = true; // TODO: Do this in observer or something
	}
}

void ecs_add_physics()
{
	ecs_scope(ecs_world(), EcsChirpObserver)
	{
		ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
			.entity = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
				.name = "InitPhysics"
			}),
			.query.terms = {
				(ecs_term_t){.id = ecs_singleton_id(EcsArgs), .inout = EcsIn},
			},
			.events = {EcsOnSet},
			.callback = init_physics,
		});
	}

	ecs_scope(ecs_world(), EcsChirpSystem)
	{
		const ecs_entity_t entity_update_physics = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "UpdatePhysics",
		});
		ecs_add_id(ecs_world(), entity_update_physics,ecs_phase(PhysicsUpdate));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_update_physics,
			.query.terms = {
				(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsWorld), .inout = EcsIn},
				(ecs_term_t){.id = ecs_singleton_id(EcsTimeStats), .inout = EcsIn},
			},
			.callback = update_physics,
		});

		const ecs_entity_t entity_sync_physics = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "SyncPhysics",
		});
		ecs_add_id(ecs_world(), entity_sync_physics, ecs_phase(PhysicsSync));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_sync_physics,
			.query.terms = {
				(ecs_term_t){.id = EcsPhysicsBody, .inout = EcsIn},
				(ecs_term_t){.id = EcsPosition, .oper = EcsOptional, .inout = EcsInOut},
				(ecs_term_t){.id = EcsProjection, .src.name = "$node", .inout = EcsInOut},
				(ecs_term_t){.src.name = "$model", .first.id = EcsChildOf, .second.name = "$this"},
				(ecs_term_t){.src.name = "$node", .first.id = EcsChildOf, .second.name = "$model"},
			},
			.callback = sync_physics,
		});
	}
}
