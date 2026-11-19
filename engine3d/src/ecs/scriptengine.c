#include "ecs.h"
#include "engineconfig.h"
#include "ecs/components.h"

#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "chirp/ecs.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"
#include "flecs/addons/script.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

static void vector3f_length_cb([[maybe_unused]] const ecs_function_ctx_t *ctx,
	[[maybe_unused]] Sint32 argc, const ecs_value_t *argv, ecs_value_t *result)
{
	SDL_assert(argv[0].type == EcsVector3f);
	const vector3f_t *self = argv[0].ptr;

	*(float*) result->ptr = b3Length(vector3f_to_b3vec3(*self));
}

static void physics_body_position_cb([[maybe_unused]] const ecs_function_ctx_t *ctx,
	[[maybe_unused]] Sint32 argc, const ecs_value_t *argv, ecs_value_t *result)
{
	SDL_assert(argv[0].type == EcsPhysicsBody);
	const b3BodyId *self = argv[0].ptr;

	*(b3Vec3*) result->ptr = b3Body_IsValid(*self)
		? b3Body_GetPosition(*self)
		: b3Vec3_zero;
}

static void physics_linear_velocity_cb([[maybe_unused]] const ecs_function_ctx_t *ctx,
	[[maybe_unused]] Sint32 argc, const ecs_value_t *argv, ecs_value_t *result)
{
	SDL_assert(argv[0].type == EcsPhysicsBody);
	const b3BodyId *self = argv[0].ptr;

	*(b3Vec3*) result->ptr = b3Body_IsValid(*self)
		? b3Body_GetLinearVelocity(*self)
		: b3Vec3_zero;
}

static void physics_rotation_cb([[maybe_unused]] const ecs_function_ctx_t *ctx,
	[[maybe_unused]] Sint32 argc, const ecs_value_t *argv, ecs_value_t *result)
{
	SDL_assert(argv[0].type == EcsPhysicsBody);
	const b3BodyId *self = argv[0].ptr;

	*(b3Quat*) result->ptr = b3Body_IsValid(*self)
		? b3Body_GetRotation(*self)
		: b3Quat_identity;
}

static void create_functions(ecs_world_t *world)
{
	ecs_scope(world, EcsChirp)
	{
		engine_config_t engine_config = engine_config_get();
		ecs_const_var_init(world, &(ecs_const_var_desc_t){
			.name = "engine_config",
			.type = EcsEngineConfig,
			.value = &engine_config,
		});

		ecs_method_init(world, &(ecs_function_desc_t){
			.name = "length",
			.parent = EcsVector3f,
			.return_type = ecs_id(ecs_f32_t),
			.callback = vector3f_length_cb,
		});

		ecs_method_init(world, &(ecs_function_desc_t){
			.name = "position",
			.parent = EcsPhysicsBody,
			.return_type = EcsVector3f,
			.callback = physics_body_position_cb,
		});

		ecs_method_init(world, &(ecs_function_desc_t){
			.name = "linear_velocity",
			.parent = EcsPhysicsBody,
			.return_type = EcsVector3f,
			.callback = physics_linear_velocity_cb,
		});

		ecs_method_init(world, &(ecs_function_desc_t){
			.name = "rotation",
			.parent = EcsPhysicsBody,
			.return_type = EcsVector4f,
			.callback = physics_rotation_cb,
		});
	}
}

static void load_main_script(ecs_iter_t *iter)
{
	create_functions(iter->world);

	const ecs_entity_t entity = ecs_script_init(iter->world, &(ecs_script_desc_t){
		.filename = "main.flecs",
		.skip_unknown = false,
	});

	if (entity == 0)
	{
		ecs_set_error("Script error", "Failed to create entity");
	}

	const EcsScript *script = ecs_get_id(iter->world, entity, ecs_id(EcsScript));
	if (script->error != nullptr)
	{
		// ecs_script_init forces error logging, so just quit here to avoid duplicate errors
		ecs_set_error("Script error", "Fatal error, check log for details");
	}
}

void ecs_add_script_engine()
{
	ecs_scope(ecs_world(), EcsChirpObserver)
	{
		ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
			.query.terms = {
				(ecs_term_t){.id = ecs_singleton_id(EcsInit), .inout = EcsInOutNone},
				(ecs_term_t){.id = ecs_singleton_id(EcsAssets), .inout = EcsInOutNone},
			},
			.events = {EcsOnSet},
			.callback = load_main_script,
		});
	}
}
