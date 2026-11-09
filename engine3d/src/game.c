#include "game.h"
#include "camera.h"
#include "ecs.h"
#include "physicsconfig.h"
#include "prefabs.h"
#include "ecs/components.h"
#include "ecs/entities.h"
#include "ecs/events.h"
#include "ui/debugoverlay.h"

#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/id.h"
#include "box3d/types.h"
#include "chirp/ecs.h"
#include "chirp/input.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

// TODO: Temporary, shouldn't be in engine logic

static constexpr auto mouse_sensitivity = 0.0015F;
static constexpr auto gamepad_sensitivity = 0.005F;
static constexpr float firepower = 10.F;

static void build_scene(ecs_iter_t *iter)
{
	const b3WorldId world_id = *ecs_field(iter, b3WorldId, 0);
	const physics_config_t *physics_config = ecs_field(iter, physics_config_t, 1);

	// Blaster

	const ecs_entity_t blaster = ecs_entity_init(iter->stage, &(ecs_entity_desc_t){
		.name = "Blaster",
	});
	ecs_set_id(ecs_world(), blaster, prefab_model("blaster"));

	const vector3f_t blaster_rotation = {.x = 0.F, .y = 90.F, .z = 0.F};
	ecs_set_id(ecs_world(), blaster, EcsRotation,
		sizeof(vector3f_t), &blaster_rotation);

	const vector3f_t blaster_position = {.x = 5.F, .y = 1.F, .z = -5.F};
	ecs_set_id(ecs_world(), blaster, EcsPosition,
		sizeof(vector3f_t), &blaster_position);

	// Scene

	const ecs_entity_t scene = ecs_new(ecs_world());
	ecs_set_id(ecs_world(), scene, prefab_scene("scene"));

	// Floor physics

	const b3BodyDef floor_def = b3DefaultBodyDef();
	b3BodyId floor_body = b3CreateBody(world_id, &floor_def);
	b3BoxHull floor_box = b3MakeBoxHull(100.F, 0.F, 100.F);
	b3ShapeDef floor_shape_def = b3DefaultShapeDef();
	floor_shape_def.baseMaterial.friction = 5.F;
	b3CreateHullShape(floor_body, &floor_shape_def, &floor_box.base);

	// Player physics

	b3BodyDef player_def = b3DefaultBodyDef();
	player_def.position = (b3Pos){.y = 1.F};
	player_def.type = b3_dynamicBody;
	player_def.motionLocks = (b3MotionLocks){
		.angularX = true,
		.angularY = true,
		.angularZ = true,
	};
	b3BodyId player_body = b3CreateBody(world_id, &player_def);
	b3Capsule player_capsule = {
		.center1 = (b3Vec3){.x = 0.F, .y = 0.F},
		.center2 = (b3Vec3){.x = 0.F, .y = 1.F},
		.radius = 1.F,
	};
	b3ShapeDef player_shape_def = b3DefaultShapeDef();
	player_shape_def.density = 1.F;
	b3CreateCapsuleShape(player_body, &player_shape_def, &player_capsule);

	const ecs_entity_t player_entity = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
		.name = "Player",
	});
	ecs_set_id(ecs_world(), player_entity, EcsPhysicsBody,
		sizeof(b3BodyId), &player_body);

	// Physics

	b3World_SetGravity(world_id, (b3Vec3){
		.y = -physics_config->gravity_y,
	});
}

static void log_spawn_position(ecs_iter_t *iter)
{
	const vector3f_t *spawn_position = ecs_field(iter, vector3f_t, 0);
	SDL_Log("Spawn: %f %f %f", spawn_position->x, spawn_position->y, spawn_position->z);
}

static void toggle_lock_cursor(ecs_iter_t *iter)
{
	const SDL_KeyboardEvent *event = ecs_field(iter, SDL_KeyboardEvent, 0);
	if (event->key != SDLK_ESCAPE || !event->down)
	{
		return;
	}

	SDL_Window *window = *ecs_field(iter, SDL_Window*, 1);

	const bool enabled = SDL_GetWindowRelativeMouseMode(window);
	SDL_SetWindowRelativeMouseMode(window, (bool) !enabled);
}

static void init_scene()
{
	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsWorld), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsConfig), .inout = EcsIn},
		},
		.events = {EcsOnSet},
		.yield_existing = true,
		.callback = build_scene,
	});

	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = EcsPosition, .inout = EcsIn},
			(ecs_term_t){
				.first.id = EcsPredEq,
				.second = (ecs_term_ref_t){
					.id = EcsIsName,
					.name = "Model.scene.Spawn",
				},
				.inout = EcsInOutNone,
			},
		},
		.events = {EcsOnSet},
		.callback = log_spawn_position,
	});

	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = EcsKeyboardEvent, .inout = EcsIn},
			(ecs_term_t){.id = EcsWindow, .src.name = "$window", .inout = EcsIn},
		},
		.events = {EcsOnKey},
		.callback = toggle_lock_cursor,
	});
}

static void init_debug_overlay(ecs_world_t *stage)
{
	const ecs_entity_t entity_draw_ui = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "DrawUi",
	});
	ecs_add_id(stage, entity_draw_ui, ecs_phase(Update));

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

static void update_camera_rotation(ecs_iter_t *iter)
{
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 0);
	if (!SDL_GetWindowRelativeMouseMode(window))
	{
		return;
	}

	const input_t input = *ecs_field(iter, input_t, 1);
	camera_t *camera = ecs_field(iter, camera_t, 2);

	const vector2f_t mouse = input_mouse_delta(input);
	camera_rotate_x(camera, -(mouse.x * mouse_sensitivity));
	camera_rotate_y(camera, -(mouse.y * mouse_sensitivity));

	camera_rotate_x(camera,
		(input_axis(input, "look_left", 0)
			- input_axis(input, "look_right", 0))
		* gamepad_sensitivity
	);
	camera_rotate_y(camera,
		(input_axis(input, "look_up", 0)
			- input_axis(input, "look_down", 0))
		* gamepad_sensitivity
	);
}

static void update_player_movement(ecs_iter_t *iter)
{
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 0);
	if (!SDL_GetWindowRelativeMouseMode(window))
	{
		return;
	}

	const b3BodyId body_id = *ecs_field(iter, b3BodyId, 1);
	const camera_t *camera = ecs_field(iter, camera_t, 3);
	const input_t input = *ecs_field(iter, input_t, 4);
	const physics_config_t physics_config = *ecs_field(iter, physics_config_t, 5);

	const b3Vec3 velocity = b3Body_GetLinearVelocity(body_id);

	vector3f_t move_forward = vector3f_normalize(camera_forward(camera));
	move_forward.y = 0.F;
	move_forward = vector3f_normalize(move_forward);

	const vector3f_t move_right = vector3f_normalize(camera_right(camera));

	vector3f_t target = vector3f_zero();

	const float axis_forward = input_axis(input, "move_forward", 0);
	if (axis_forward > 0.F)
	{
		target = vector3f_add(target, vector3f_scale(move_forward, axis_forward));
	}

	const float axis_backward = input_axis(input, "move_backward", 0);
	if (axis_backward > 0.F)
	{
		target = vector3f_sub(target, vector3f_scale(move_forward, axis_backward));
	}

	const float axis_left = input_axis(input, "move_left", 0);
	if (axis_left > 0.F)
	{
		target = vector3f_sub(target, vector3f_scale(move_right, axis_left));
	}

	const float axis_right = input_axis(input, "move_right", 0);
	if (axis_right > 0.F)
	{
		target = vector3f_add(target, vector3f_scale(move_right, axis_right));
	}

	if (!vector3f_is_zero(target))
	{
		target = vector3f_scale(target, physics_config.move_speed);
		target.y = velocity.y;
	}

	if (input_is_pressed(input, "jump", 0)
		&& velocity.y > -0.1F && velocity.y < 0.1F)
	{
		target.y = physics_config.jump_speed;
	}

	if (!vector3f_is_zero(target))
	{
		b3Body_SetLinearVelocity(body_id, vector3f_to_b3vec3(target));
	}
}

static void update_player_shoot(ecs_iter_t *iter)
{
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 0);
	if (!SDL_GetWindowRelativeMouseMode(window))
	{
		return;
	}

	const input_t input = *ecs_field(iter, input_t, 1);
	if (!input_is_pressed(input, "shoot", 0))
	{
		return;
	}

	const vector3f_t position = *ecs_field(iter, vector3f_t, 2);
	const b3WorldId world_id = *ecs_field(iter, b3WorldId, 4);
	const camera_t *camera = ecs_field(iter, camera_t, 5);

	const ecs_entity_t entity = ecs_new(iter->stage);
	ecs_set_id(iter->stage, entity, prefab_model("bullet"));

	ecs_set_id(iter->stage, entity, EcsPosition,
		sizeof(vector3f_t), &position);

	const vector3f_t rotation = vector3f_zero();
	ecs_set_id(iter->stage, entity, EcsRotation,
		sizeof(vector3f_t), &rotation);

	const vector3f_t forward = vector3f_normalize(vector3f_sub(camera->target, position));
	const vector3f_t velocity = vector3f_scale(forward, firepower);

	// TODO: Make cylinder
	// TODO: Fix rotation

	b3BodyDef body_def = b3DefaultBodyDef();
	body_def.position = vector3f_to_b3vec3(position);
	body_def.linearVelocity = vector3f_to_b3vec3(velocity);
	body_def.type = b3_dynamicBody;
	b3BodyId body = b3CreateBody(world_id, &body_def);
	b3BoxHull box = b3MakeCubeHull(1.F);
	b3ShapeDef shape_def = b3DefaultShapeDef();
	shape_def.density = 1.F;
	shape_def.baseMaterial.friction = 0.3F;
	b3CreateHullShape(body, &shape_def, &box.base);

	ecs_set_id(iter->stage, entity, EcsPhysicsBody,
		sizeof(b3BodyId), &body);
}

static void update_camera_position(ecs_iter_t *iter)
{
	const b3BodyId body_id = *ecs_field(iter, b3BodyId, 0);
	camera_t *camera = ecs_field(iter, camera_t, 2);

	const b3Vec3 player_position = b3Body_GetPosition(body_id);
	const vector3f_t camera_dir = vector3f_sub(camera->target, camera->position);
	camera->position = vector3f_from_b3vec3(player_position);
	camera->target = vector3f_add(camera->position, camera_dir);
}

static void update_weapon_projection(ecs_iter_t *iter)
{
	const camera_t *camera = ecs_field(iter, camera_t, 0);
	vector3f_t *position = ecs_field(iter, vector3f_t, 1);
	vector3f_t *rotation = ecs_field(iter, vector3f_t, 2);
	projection_t *projection = ecs_field(iter, projection_t, 6);

	const vector3f_t forward_n = vector3f_normalize(vector3f_sub(camera->target, camera->position));
	const vector3f_t right_n = vector3f_normalize(vector3f_cross(forward_n, camera->up));
	const vector3f_t up_n = vector3f_normalize(camera->up);

	vector3f_t weapon_position = camera->position;
	weapon_position = vector3f_add(weapon_position, vector3f_scale(forward_n, 0.2F));
	weapon_position = vector3f_add(weapon_position, vector3f_scale(right_n, 0.25F));
	weapon_position = vector3f_add(weapon_position, vector3f_scale(up_n, -0.2F));

	*position = weapon_position;
	*rotation = (vector3f_t){
		.x = SDL_asinf(forward_n.y),
		.y = SDL_atan2f(-forward_n.z, forward_n.x) - (SDL_PI_F * 0.5F),
		.z = 0.0F,
	};
	projection->rebuild = true; // TODO: Maybe do this in an observer or something
}

static void init_player_input(ecs_world_t *stage)
{
	const ecs_entity_t entity_update_camera_rotation = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdateCameraRotation",
	});
	ecs_add_id(stage, entity_update_camera_rotation, ecs_phase(Update));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_camera_rotation,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsInput), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsInOut},
		},
		.callback = update_camera_rotation,
	});

	const ecs_entity_t entity_update_player_movement = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdatePlayerMovement",
	});
	ecs_add_id(stage, entity_update_player_movement, ecs_phase(Update));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_player_movement,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = EcsPhysicsBody, .src.name = "$player", .inout = EcsIn},
			(ecs_term_t){
				.src.name = "$player",
				.first.id = EcsPredEq,
				.second = (ecs_term_ref_t){.id = EcsIsName, .name = "Player"},
				.inout = EcsInOutNone,
			},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsInput), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsConfig), .inout = EcsIn},
		},
		.callback = update_player_movement,
	});

	const ecs_entity_t entity_update_player_shoot = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdatePlayerShoot",
	});
	ecs_add_id(stage, entity_update_player_shoot, ecs_phase(Update));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_player_shoot,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsInput), .inout = EcsIn},
			(ecs_term_t){.id = EcsPosition, .src.name = "$blaster", .inout = EcsIn},
			(ecs_term_t){
				.src.name = "$blaster",
				.first.id = EcsPredEq,
				.second = (ecs_term_ref_t){.id = EcsIsName, .name = "Blaster"},
				.inout = EcsInOutNone,
			},
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsWorld), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsIn},
		},
		.callback = update_player_shoot,
	});

	const ecs_entity_t entity_update_camera_position = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdatePlayerCamera",
	});
	ecs_add_id(stage, entity_update_camera_position, ecs_phase(Update));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_camera_position,
		.query.terms = {
			(ecs_term_t){.id = EcsPhysicsBody, .src.name = "$player", .inout = EcsIn},
			(ecs_term_t){
				.src.name = "$player",
				.first.id = EcsPredEq,
				.second = (ecs_term_ref_t){.id = EcsIsName, .name = "Player"},
				.inout = EcsInOutNone,
			},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsInOut},
		},
		.callback = update_camera_position,
	});

	const ecs_entity_t entity_update_weapon_projection = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdateWeaponProjection",
	});
	ecs_add_id(stage, entity_update_weapon_projection, ecs_phase(Update));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_weapon_projection,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsIn},
			(ecs_term_t){.id = EcsPosition, .inout = EcsInOut},
			(ecs_term_t){.id = EcsRotation, .inout = EcsInOut},
			(ecs_term_t){
				.first.id = EcsPredEq,
				.second = (ecs_term_ref_t){.id = EcsIsName, .name = "Blaster"},
				.inout = EcsInOutNone,
			},
			(ecs_term_t){
				.src.name = "$model",
				.first.id = EcsChildOf,
				.second.name = "$this",
			},
			(ecs_term_t){
				.src.name = "$node",
				.first.id = EcsChildOf,
				.second.name = "$model",
			},
			(ecs_term_t){.id = EcsProjection, .src.name = "$node"},
		},
		.callback = update_weapon_projection,
	});
}

void game_init(ecs_world_t *stage)
{
	init_scene();
	init_debug_overlay(stage);
	init_player_input(stage);
}
