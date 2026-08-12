#include "appstate.h"
#include "args.h"
#include "camera.h"
#include "cast.h"
#include "ecs.h"
#include "model.h"
#include "nkui.h"
#include "physics.h"
#include "physicsconfig.h"
#include "prefabs.h"
#include "scriptengine.h"
#include "termcolors.h"
#include "timestats.h"
#include "ecs/components.h"
#include "ecs/events.h"
#include "ui/debugoverlay.h"

#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/collision.h"
#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "box3d/types.h"
#include "chirp/assets.h"
#include "chirp/ecs.h"
#include "chirp/input.h"
#include "chirp/logcategory.h"
#include "chirp/systeminfo.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_version.h>
#include <SDL3/SDL_video.h>

static constexpr auto mouse_sensitivity = 0.0015F;

[[nodiscard]]
static bool fatal_error_message_box()
{
	const args_t *args = ecs_get_id(ecs_world(), EcsArgs, EcsArgs);
	if (args == nullptr || args->fatal_error_message_box == OPT_NOT_SET)
	{
#ifdef NDEBUG
		return false;
#else
		return true;
#endif
	}
	return args->fatal_error_message_box == OPT_ENABLE;
}

static SDL_AppResult fatal_error(const char *message)
{
	SDL_LogCritical(LOG_CATEGORY_CORE, "%s: %s", message, SDL_GetError());

	if (fatal_error_message_box())
	{
		SDL_Window *window = *(SDL_Window**) ecs_get_mut_id(ecs_world(), ecs_singleton(EcsWindow));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			message, SDL_GetError(), window);
	}

	return SDL_APP_FAILURE;
}

[[nodiscard]]
static bool sdl_supported()
{
	const int linked = SDL_GetVersion();

	if (linked < SDL_VERSIONNUM(3, 4, 0))
	{
		return SDL_SetError("SDL 3.4 or newer is required");
	}

	// Micro is bugfixes only, so just ignore it
	if (SDL_VERSIONNUM_MAJOR(linked) != SDL_MAJOR_VERSION
		|| SDL_VERSIONNUM_MINOR(linked) != SDL_MINOR_VERSION)
	{
		SDL_LogWarn(LOG_CATEGORY_CORE,
			"Binary is compiled against SDL %d.%d, but running against SDL %d.%d",
			SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
			SDL_VERSIONNUM_MAJOR(linked), SDL_VERSIONNUM_MINOR(linked)
		);
	}

	return true;
}

static void log_spawn_position(ecs_iter_t *iter)
{
	const position_t *spawn_position = ecs_field(iter, position_t, 0);
	SDL_Log("Spawn: %f %f %f", spawn_position->x, spawn_position->y, spawn_position->z);
}

static void build_scene(ecs_iter_t *iter)
{
	const b3WorldId physics_world = *ecs_field(iter, b3WorldId, 0);
	const physics_config_t *physics_config = ecs_field(iter, physics_config_t, 1);

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

	// Blaster

	const ecs_entity_t blaster = ecs_new(ecs_world());
	ecs_set_id(ecs_world(), blaster, prefab_model("blaster"));
	ecs_set_name(ecs_world(), blaster, "Blaster");

	const rotation_t rotation = {
		.x = 0.F,
		.y = 90.F,
		.z = 0.F,
	};
	ecs_set_id(ecs_world(), blaster, EcsRotation,
		sizeof(rotation_t), &rotation);

	const position_t position = {
		.x = 5.F,
		.y = 1.F,
		.z = -5.F,
	};
	ecs_set_id(ecs_world(), blaster, EcsPosition,
		sizeof(position_t), &position);

	// Scene

	const ecs_entity_t scene = ecs_new(ecs_world());
	ecs_set_id(ecs_world(), scene, prefab_scene("scene"));

	// Physics

	const b3BodyDef floor_def = b3DefaultBodyDef();
	b3BodyId floor_body = b3CreateBody(physics_world, &floor_def);
	b3BoxHull floor_box = b3MakeBoxHull(100.F, 0.F, 100.F);
	b3ShapeDef floor_shape_def = b3DefaultShapeDef();
	floor_shape_def.baseMaterial.friction = 5.F;
	b3CreateHullShape(floor_body, &floor_shape_def, &floor_box.base);

	// TODO: Make character mover
	b3BodyDef player_def = b3DefaultBodyDef();
	player_def.position = (b3Pos){.y = 1.F};
	player_def.type = b3_dynamicBody;
	player_def.motionLocks = (b3MotionLocks){
		.angularX = true,
		.angularY = true,
		.angularZ = true,
	};
	b3BodyId player_body = b3CreateBody(physics_world, &player_def);
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

	b3World_SetGravity(physics_world, (b3Vec3){
		.y = -physics_config->gravity_y,
	});
}

static void toggle_lock_cursor(ecs_iter_t *iter)
{
	const SDL_KeyboardEvent *event = ecs_field(iter, SDL_KeyboardEvent, 0);
	if (event->key != SDLK_ESCAPE || !event->down)
	{
		return;
	}

	SDL_Window *window = *ecs_field(iter, window_t*, 1);

	const bool enabled = SDL_GetWindowRelativeMouseMode(window);
	SDL_SetWindowRelativeMouseMode(window, (bool) !enabled);
}

static void set_default_metadata()
{
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, ENGINE_NAME);
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, ENGINE_VERSION);
	SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, ENGINE_IDENTIFIER);
}

static void set_log_priorities(const SDL_LogPriority *priorities)
{
	const SDL_LogPriority priority_all = priorities[LOG_CATEGORY_COUNT];
	if (priority_all == SDL_LOG_PRIORITY_INVALID)
	{
#ifdef NDEBUG
		SDL_SetLogPriorities(SDL_LOG_PRIORITY_INFO);
#else
		SDL_SetLogPriorities(SDL_LOG_PRIORITY_VERBOSE);
#endif
	}
	else
	{
		SDL_SetLogPriorities(priority_all);
	}

	for (int i = 0; i < LOG_CATEGORY_COUNT; i++)
	{
		const SDL_LogPriority priority = priorities[i];
		if (priority != SDL_LOG_PRIORITY_INVALID)
		{
			SDL_SetLogPriority(i, priority);
		}
	}
}

static void log_callback(void *userdata, const int category,
	const SDL_LogPriority priority, const char *message)
{
	const char *category_name = log_category_name(category);

	static constexpr size_t temp_len = 256;
	static char temp[temp_len];

	SDL_snprintf(temp, temp_len, COLOR_FG_BOLD("%-7s") " " COLOR_FG_WHITE("%s"), category_name, message);
	SDL_GetDefaultLogOutputFunction()(userdata, category, priority, temp);
}

SDL_AppResult SDL_AppInit(void **appstate, const int argc, char **argv)
{
	if (!system_info_cpu_supported())
	{
		return fatal_error("Unsupported CPU");
	}

	if (!sdl_supported())
	{
		return fatal_error("Unsupported SDL version");
	}

	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG, COLOR_FG_BLUE("debug "));
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, COLOR_FG_GREEN("info  "));
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN, COLOR_FG_YELLOW("warn  "));
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR, COLOR_FG_RED("error "));
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, COLOR_FG_RED("fatal "));
	SDL_SetLogOutputFunction(log_callback, nullptr);

	args_t args;
	if (!args_parse(argc, argv, &args))
	{
		return SDL_APP_SUCCESS;
	}

	set_log_priorities(args.log_priorities);

	SDL_LogDebug(LOG_CATEGORY_CORE, "Assertion level: %s",
#if SDL_ASSERT_LEVEL == 0
		"disabled"
#elif SDL_ASSERT_LEVEL == 1
		"release"
#elif SDL_ASSERT_LEVEL == 2
		"debug"
#elif SDL_ASSERT_LEVEL == 3
		"paranoid"
#endif
	);

	if (args.video_driver != nullptr
		&& !SDL_SetHint(SDL_HINT_VIDEO_DRIVER, args.video_driver))
	{
		SDL_LogError(LOG_CATEGORY_CORE, "Failed to set hint: %s", SDL_GetError());
	}

	if (args.audio_driver != nullptr
		&& !SDL_SetHint(SDL_HINT_AUDIO_DRIVER, args.audio_driver))
	{
		SDL_LogError(LOG_CATEGORY_CORE, "Failed to set hint: %s", SDL_GetError());
	}

	if (args.allow_screensaver != OPT_NOT_SET
		&& !SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER,
			arg_option_str(args.allow_screensaver)))
	{
		SDL_LogWarn(LOG_CATEGORY_CORE, "Failed to set hint: %s", SDL_GetError());
	}

	app_state_t *state = SDL_calloc(1, sizeof(app_state_t));
	if (state == nullptr)
	{
		return fatal_error("Memory allocation failed");
	}
	*appstate = state;

	ecs_create();

	ecs_scope(ecs_world(), EcsChirpModule)
	{
		ecs_add_gpu();
		ecs_add_window();
		ecs_add_assets();
		ecs_add_physics();
		ecs_add_render();
		ecs_add_script_engine();
		ecs_add_models();
		ecs_add_nkui();
		ecs_add_input();
		ecs_add_logging();
	}

	ecs_set_id(ecs_world(), ecs_singleton(EcsArgs),
		sizeof(args_t), &args);

	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsWorld), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsPhysicsConfig), .inout = EcsIn},
		},
		.events = {EcsOnSet},
		.callback = build_scene,
	});

	// Should be changed by assets later when loaded
	set_default_metadata();

	constexpr SDL_InitFlags init_flags = SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD;
	if (!SDL_Init(init_flags))
	{
		return fatal_error("Initialisation failed");
	}

	ecs_set_id(ecs_world(), ecs_singleton(EcsInit),
		sizeof(SDL_InitFlags), &init_flags);

	const time_stats_t time_stats = {
		.last_update = SDL_GetTicksNS(),
	};

	ecs_set_id(ecs_world(), ecs_singleton(EcsTimeStats),
		sizeof(time_stats_t), &time_stats);

	const camera_t camera = camera_create_default();
	ecs_set_id(ecs_world(), ecs_singleton(EcsCamera),
		sizeof(camera_t), &camera);

	const physics_config_t physics_config = physics_config_create_default();
	ecs_set_id(ecs_world(), ecs_singleton(EcsPhysicsConfig),
		sizeof(physics_config_t), &physics_config);

	const SDL_FColor clear_color = {.r = 0.12F, .g = 0.12F, .b = 0.12F, .a = 1.F};
	ecs_set_id(ecs_world(), ecs_singleton(EcsClearColor),
		sizeof(clear_color_t), &clear_color);

	state->status_query = ecs_query_init(ecs_world(), &(ecs_query_desc_t){
		.terms = {
			(ecs_term_t){.id = EcsError},
		},
	});

	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = EcsKeyboardEvent, .inout = EcsIn},
			(ecs_term_t){.id = EcsWindow, .src.name = "$window", .inout = EcsIn},
		},
		.events = {EcsOnKey},
		.callback = toggle_lock_cursor,
	});

	ecs_system_init(ecs_world(), &(ecs_system_desc_t){
		.entity = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "DrawUi",
			.add = ecs_ids(ecs_dependson(ecs_phase(PHASE_UPDATE))),
		}),
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsNkContext), .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsCamera), .inout = EcsIn},
			(ecs_term_t){.id = EcsPhysicsBody, .src.name = "$plr_bdy", .inout = EcsIn},
			(ecs_term_t){.id = ecs_singleton_id(EcsWindow), .inout = EcsInOut},
			(ecs_term_t){.id = ecs_singleton_id(EcsGpuDevice), .inout = EcsInOut},
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

	// NOTE: Returning from here will call a default SDL_Init if not already called
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
	app_state_t *state = appstate;

	time_stats_t *time_stats = ecs_get_mut_id(ecs_world(),
		ecs_singleton(EcsTimeStats));

	const Uint64 current_update = SDL_GetTicksNS();
	time_stats->dt = (float) (current_update - time_stats->last_update) / SDL_NS_PER_SECOND;
	time_stats->last_update = current_update;

	time_stats->count++;
	time_stats->duration += time_stats->dt;
	if (time_stats->duration >= 1.F)
	{
		time_stats->fps = time_stats->count;
		time_stats->count = 0;
		time_stats->duration = 0;
	}

	ecs_progress(ecs_world(), time_stats->dt);

	const physics_config_t *physics_config = ecs_get_id(ecs_world(),
		ecs_singleton(EcsPhysicsConfig));

	const ecs_entity_t player_entity = ecs_lookup(ecs_world(), "Player");
	const b3BodyId *player_body_id = player_entity != 0
		? ecs_get_id(ecs_world(), player_entity, EcsPhysicsBody)
		: nullptr;

	SDL_Window *window = *(SDL_Window**) ecs_get_mut_id(ecs_world(), ecs_singleton(EcsWindow));
	camera_t *camera = ecs_get_mut_id(ecs_world(), ecs_singleton(EcsCamera));

	if (SDL_GetWindowRelativeMouseMode(window)
		&& player_body_id != nullptr)
	{
		const input_t input = *(const input_t*) ecs_get_id(ecs_world(), ecs_singleton(EcsInput));

		vector2f_t mouse;
		SDL_GetRelativeMouseState(&mouse.x, &mouse.y);
		camera_rotate_x(camera, -(mouse.x * mouse_sensitivity));
		camera_rotate_y(camera, -(mouse.y * mouse_sensitivity));

		const float move_speed = physics_config->move_speed;
		const float jump_speed = physics_config->jump_speed;

		b3Vec3 player_velocity = b3Body_GetLinearVelocity(*player_body_id);

		if (input_axis(input, "move_forward", 0) > 0.F)
		{
			const vector3f_t velocity = camera_to_z(camera, move_speed * time_stats->dt);
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_axis(input, "move_backward", 0) > 0.F)
		{
			const vector3f_t velocity = camera_to_z(camera, -(move_speed * time_stats->dt));
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_axis(input, "move_left", 0) > 0.F)
		{
			const vector3f_t velocity = camera_to_x(camera, -(move_speed * time_stats->dt));
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_axis(input, "move_right", 0) > 0.F)
		{
			const vector3f_t velocity = camera_to_x(camera, move_speed * time_stats->dt);
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_is_down(input, "move_up", 0))
		{
			const vector3f_t velocity = camera_to_y(camera, move_speed * time_stats->dt);
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_is_down(input, "move_down", 0))
		{
			const vector3f_t velocity = camera_to_y(camera, -(move_speed * time_stats->dt));
			player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
		}

		if (input_is_pressed(input, "jump", 0))
		{
			if (player_velocity.y > -0.1F && player_velocity.y < 0.1F)
			{
				const b3Vec3 velocity = {.y = jump_speed};
				player_velocity = b3Add(player_velocity, cast(b3Vec3, velocity));
			}
		}

		b3Body_SetLinearVelocity(*player_body_id, player_velocity);

		if (input_is_pressed(input, "shoot", 0))
		{
			static constexpr float firepower = 10.F;

			const ecs_entity_t entity = ecs_new(ecs_world());
			ecs_set_id(ecs_world(), entity, prefab_model("bullet"));

			static ecs_query_t *position_query = nullptr;
			if (position_query == nullptr)
			{
				position_query = ecs_query_init(ecs_world(), &(ecs_query_desc_t){
					.terms = {
						(ecs_term_t){
							.id = EcsPosition,
							.src.id = ecs_lookup(ecs_world(), "Blaster"),
						},
					},
				});
			}

			position_t position = {};
			ecs_iter_t iter = ecs_query_iter(ecs_world(), position_query);
			while (ecs_query_next(&iter)) // Should only have 1 match, but just to be sure
			{
				position = *ecs_field(&iter, position_t, 0);
			}

			ecs_set_id(ecs_world(), entity, EcsPosition,
				sizeof(position_t), &position);

			const rotation_t rotation = vector3f_zero();
			ecs_set_id(ecs_world(), entity, EcsRotation,
				sizeof(rotation_t), &rotation);

			const b3WorldId *physics_world = ecs_get_id(ecs_world(),
				EcsPhysicsWorld, EcsPhysicsWorld);

			const vector3f_t forward = vector3f_normalize(vector3f_sub(camera->target, position));
			const vector3f_t velocity = vector3f_scale(forward, firepower);

			// TODO: Make cylinder
			// TODO: Fix rotation

			b3BodyDef body_def = b3DefaultBodyDef();
			body_def.position = cast(b3Pos, position);
			body_def.linearVelocity = cast(b3Vec3, velocity);
			body_def.type = b3_dynamicBody;
			b3BodyId body = b3CreateBody(*physics_world, &body_def);
			b3BoxHull box = b3MakeCubeHull(1.F);
			b3ShapeDef shape_def = b3DefaultShapeDef();
			shape_def.density = 1.F;
			shape_def.baseMaterial.friction = 0.3F;
			b3CreateHullShape(body, &shape_def, &box.base);

			ecs_set_id(ecs_world(), entity, EcsPhysicsBody,
				sizeof(b3BodyId), &body);
		}
	}

	// TODO: Set clamping during creation
	const b3Vec3 min_velocity =
	{
		.x = -physics_config->max_move_speed,
		.y = -1'000.F,
		.z = -physics_config->max_move_speed,
	};
	const b3Vec3 max_velocity =
	{
		.x = physics_config->max_move_speed,
		.y = 1'000.F,
		.z = physics_config->max_move_speed,
	};
	const b3Vec3 velocity = b3Body_GetLinearVelocity(*player_body_id);
	const b3Vec3 clamped_velocity = b3Clamp(velocity, min_velocity, max_velocity);
	b3Body_SetLinearVelocity(*player_body_id, clamped_velocity);

	// TODO: There should be a better way to do this, right?
	const b3Vec3 player_position = b3Body_GetPosition(*player_body_id);
	camera->target = vector3f_add(camera->target, vector3f_sub(cast(vector3f_t, player_position), camera->position));
	camera->position = cast(vector3f_t, player_position);

	const vector3f_t forward_n = vector3f_normalize(vector3f_sub(camera->target, camera->position));
	const vector3f_t right_n = vector3f_normalize(vector3f_cross(forward_n, camera->up));
	const vector3f_t up_n = vector3f_normalize(camera->up);

	vector3f_t weapon_position = camera->position;
	weapon_position = vector3f_add(weapon_position, vector3f_scale(forward_n, 0.2F));
	weapon_position = vector3f_add(weapon_position, vector3f_scale(right_n, 0.25F));
	weapon_position = vector3f_add(weapon_position, vector3f_scale(up_n, -0.2F));

	static ecs_query_t *weapon_query = nullptr;
	if (weapon_query == nullptr)
	{
		weapon_query = ecs_query_init(ecs_world(), &(ecs_query_desc_t){
			.terms = {
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
		});
	}

	ecs_iter_t iter = ecs_query_iter(ecs_world(), weapon_query);
	while (ecs_query_next(&iter))
	{
		vector3f_t *positions = ecs_field(&iter, position_t, 0);
		vector3f_t *rotations = ecs_field(&iter, rotation_t, 1);
		projection_t *projections = ecs_field(&iter, projection_t, 5);

		positions[0] = weapon_position;
		rotations[0] = (rotation_t){
			.x = SDL_asinf(forward_n.y),
			.y = SDL_atan2f(-forward_n.z, forward_n.x) - (SDL_PI_F * 0.5F),
			.z = 0.0F,
		};
		projections[0].rebuild = true; // TODO: Maybe do this in an observer or something
	}

	iter = ecs_query_iter(ecs_world(), state->status_query);
	if (ecs_query_next(&iter))
	{
		const error_t *error = ecs_field(&iter, error_t, 0);
		SDL_LogCritical(LOG_CATEGORY_CORE, "%s: %s", error->title, error->message);
		if (fatal_error_message_box())
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				error->title, error->message, window);
		}
		ecs_iter_fini(&iter);
		return SDL_APP_FAILURE;
	}

	return SDL_APP_CONTINUE;
}

static void emit(const ecs_entity_t event, const ecs_id_t value_type, void *value)
{
	const ecs_entity_t entity = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
		.set = ecs_values((ecs_value_t){.type = value_type, .ptr = value}),
	});

	ecs_emit(ecs_world(), &(ecs_event_desc_t){
		.event = event,
		.entity = entity,
		.ids = &(ecs_type_t){
			.array = (ecs_id_t[]){value_type},
			.count = 1,
		},
	});

	ecs_delete(ecs_world(), entity);
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void *appstate, SDL_Event *event)
{
	const SDL_EventType event_type = event->type;

	if (event_type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}

	nkui_context_t *nkui_context = ecs_get_mut_id(ecs_world(),
		EcsNkContext, EcsNkContext);
	if (nkui_context != nullptr)
	{
		nkui_handle_event(nkui_context, event);
	}

	if (event_type == SDL_EVENT_MOUSE_BUTTON_DOWN
		|| event_type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		emit(EcsOnMouseButton, EcsMouseButtonEvent, &event->button);
	}

	if (event_type == SDL_EVENT_KEY_DOWN
		|| event_type == SDL_EVENT_KEY_UP)
	{
		emit(EcsOnKey, EcsKeyboardEvent, &event->key);
	}

	if (event->type == SDL_EVENT_WINDOW_RESIZED)
	{
		emit(EcsOnWindowResized, EcsWindowEvent, &event->window);
	}

	if (event->type == SDL_EVENT_GAMEPAD_ADDED)
	{
		input_gamepad_open(event->gdevice.which);
	}
	else if (event->type == SDL_EVENT_GAMEPAD_REMOVED)
	{
		input_gamepad_close(event->gdevice.which);
	}

	const input_t input = *(const input_t*) ecs_get_id(ecs_world(), ecs_singleton(EcsInput));
	input_update(input, event);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, [[maybe_unused]] SDL_AppResult result)
{
	if (ecs_world() == nullptr)
	{
		SDL_free(appstate);
		return;
	}

	ecs_query_t *query = ecs_query_init(ecs_world(), &(ecs_query_desc_t){
		.terms = {
			(ecs_term_t){.id = EcsModel, .inout = EcsIn},
		},
	});
	ecs_iter_t iter = ecs_query_iter(ecs_world(), query);
	while (ecs_query_next(&iter))
	{
		model_t *models = ecs_field(&iter, model_t, 0);
		for (Sint32 i = 0; i < iter.count; i++)
		{
			model_destroy(models + i);
		}
	}
	ecs_query_fini(query);

	assets_destroy(ecs_get_id(ecs_world(), ecs_singleton(EcsAssets)));
	script_engine_destroy();
	physics_destroy(*(b3WorldId*) ecs_get_id(ecs_world(), ecs_singleton(EcsPhysicsWorld)));

	SDL_Window *window = *(SDL_Window**) ecs_get_mut_id(ecs_world(),
		ecs_singleton(EcsWindow));

	SDL_GPUDevice *gpu_device = *(SDL_GPUDevice**) ecs_get_mut_id(ecs_world(),
		ecs_singleton(EcsGpuDevice));

	SDL_GPUGraphicsPipeline *pipeline = *(SDL_GPUGraphicsPipeline**) ecs_get_mut_id(ecs_world(),
		ecs_singleton(EcsGpuGraphicsPipeline));

	SDL_GPUTexture *depth_texture = *(SDL_GPUTexture**) ecs_get_mut_id(ecs_world(),
		ecs_singleton(EcsDepthTexture));

	SDL_ReleaseGPUTexture(gpu_device, depth_texture);
	SDL_ReleaseGPUGraphicsPipeline(gpu_device, pipeline);
	SDL_ReleaseWindowFromGPUDevice(gpu_device, window);

	SDL_DestroyWindow(window);
	SDL_DestroyGPUDevice(gpu_device);

	ecs_destroy();
	SDL_free(appstate);
}
