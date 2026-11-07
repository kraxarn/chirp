#include "game.h"
#include "camera.h"
#include "ecs.h"
#include "ecs/components.h"
#include "ui/debugoverlay.h"

#include "flecs.h"
#include "chirp/ecs.h"
#include "chirp/input.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_video.h>

// TODO: Temporary, shouldn't be in engine logic

static constexpr auto mouse_sensitivity = 0.0015F;
static constexpr auto gamepad_sensitivity = 0.005F;

static void init_debug_overlay(ecs_world_t *stage)
{
	const ecs_entity_t entity_draw_ui = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "DrawUi",
	});
	ecs_add_id(stage, entity_draw_ui,
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

static void update_camera_rotation(ecs_iter_t *iter)
{
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 0);
	if (!SDL_GetWindowRelativeMouseMode(window))
	{
		return;
	}

	const input_t input = *ecs_field(iter, input_t, 1);
	camera_t *camera = ecs_field(iter, camera_t, 2);

	vector2f_t mouse;
	SDL_GetRelativeMouseState(&mouse.x, &mouse.y);
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

static void init_player_input(ecs_world_t *stage)
{
	const ecs_entity_t entity_update_camera_rotation = ecs_entity_init(stage, &(ecs_entity_desc_t){
		.name = "UpdateCameraRotation",
	});
	ecs_add_id(stage, entity_update_camera_rotation,
		ecs_dependson(ecs_phase(PHASE_UPDATE)));

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = entity_update_camera_rotation,
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsInput), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsInOut},
		},
		.callback = update_camera_rotation,
	});
}

void game_init(ecs_world_t *stage)
{
	init_debug_overlay(stage);
	init_player_input(stage);
}
