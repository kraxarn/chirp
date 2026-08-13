#include "ecs.h"
#include "args.h"
#include "camera.h"
#include "model.h"
#include "nkui.h"
#include "physicsconfig.h"
#include "timestats.h"
#include "ecs/components.h"
#include "ecs/entities.h"
#include "ecs/events.h"
#include "ecs/tags.h"

#include "chirp/ecs.h"
#include "flecs.h"
#include "box3d/id.h"
#include "chirp/ecsosapi.h"
#include "chirp/ecsutils.h"
#include "chirp/logcategory.h"
#include "chirp/windowconfig.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_cpuinfo.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>

static ecs_entity_t phases[PHASE_COUNT];

static ecs_entity_t phase(const char *name)
{
	const ecs_entity_desc_t entity_desc = {
		.name = name,
		.add = ecs_ids(EcsPhase),
	};
	return ecs_entity_init(ecs_world(), &entity_desc);
}

static void phase_depend(const phase_t source, const phase_t target)
{
	ecs_add_pair(ecs_world(), phases[source], EcsDependsOn, phases[target]);
}

static void create_pipeline()
{
	const ecs_pipeline_desc_t pipeline_desc = {
		.query.terms = {
			(ecs_term_t){
				.id = EcsSystem,
			},
			(ecs_term_t){
				.id = EcsPhase,
				.src.id = EcsCascade,
				.trav = EcsDependsOn,
			},
		},
	};
	const ecs_entity_t pipeline = ecs_pipeline_init(ecs_world(), &pipeline_desc);
	ecs_set_pipeline(ecs_world(), pipeline);

	phases[PHASE_UPDATE_BEGIN] = phase("UpdateBegin");
	phases[PHASE_UPDATE] = phase("Update");
	phases[PHASE_UPDATE_END] = phase("UpdateEnd");
	phases[PHASE_PHYSICS_UPDATE] = phase("PhysicsUpdate");
	phases[PHASE_PHYSICS_SYNC] = phase("PhysicsSync");
	phases[PHASE_RENDER_BEGIN] = phase("RenderBegin");
	phases[PHASE_RENDER] = phase("Render");
	phases[PHASE_RENDER_END] = phase("RenderEnd");

	phase_depend(PHASE_UPDATE, PHASE_UPDATE_BEGIN);
	phase_depend(PHASE_UPDATE_END, PHASE_UPDATE);
	phase_depend(PHASE_PHYSICS_UPDATE, PHASE_UPDATE_END);
	phase_depend(PHASE_PHYSICS_SYNC, PHASE_PHYSICS_UPDATE);
	phase_depend(PHASE_RENDER_BEGIN, PHASE_PHYSICS_SYNC);
	phase_depend(PHASE_RENDER, PHASE_RENDER_BEGIN);
	phase_depend(PHASE_RENDER_END, PHASE_RENDER);
}

[[nodiscard]]
static ecs_entity_t entity(const char *name)
{
	return ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
		.name = name,
	});
}

static ecs_entity_t tag(const char *name)
{
	const ecs_entity_desc_t entity_desc = {
		.name = name,
	};

	const ecs_entity_t entity = ecs_entity_init(ecs_world(), &entity_desc);
	SDL_assert(entity != 0);
	return entity;
}

static void add_events()
{
	EcsOnMouseButton = entity("OnMouseButton");
	EcsMouseButtonEvent = component("MouseButtonEvent", SDL_MouseButtonEvent);

	EcsOnKey = entity("OnKey");
	EcsKeyboardEvent = component("KeyboardEvent", SDL_KeyboardEvent);

	EcsOnWindowResized = entity("OnWindowResized");
	EcsWindowEvent = component("WindowEvent", SDL_WindowEvent);
}

static void add_modules()
{
	ecs_scope(ecs_world(), EcsChirpModule)
	{
		EcsInstanceOf = entity("InstanceOf");

		EcsScene = tag("Scene");

		EcsTimeStats = component("TimeStats", time_stats_t);
		EcsWindowConfig = component("WindowConfig", window_config_t);
		EcsWindow = component("Window", window_t*);
		EcsGpuDevice = component("GpuDevice", gpu_device_t*);
		EcsGpuGraphicsPipeline = component("GpuGraphicsPipeline", gpu_graphics_pipeline_t*);
		EcsDepthTexture = component("DepthTexture", depth_texture_t*);
		EcsGpuCommandBuffer = component("GpuCommandBuffer", gpu_command_buffer_t*);
		EcsGpuRenderPass = component("GpuRenderPass", gpu_render_pass_t*);
		EcsSwapchainTexture = component("SwapchainTexture", swapchain_texture_t*);
		EcsSwapchainTextureSize = component("SwapchainTextureSize", swapchain_texture_size_t);
		EcsCamera = component("Camera", camera_t);
		EcsPhysicsConfig = component("PhysicsConfig", physics_config_t);
		EcsModel = component("Model", model_t);
		EcsRotation = component("Rotation", rotation_t);
		EcsPosition = component("Position", position_t);
		EcsScale = component("Scale", scale_t);
		EcsProjection = component("Projection", projection_t);
		EcsNkContext = component("NkContext", nkui_context_t);
		EcsVertexShader = component("VertexShader", vertex_shader_t*);
		EcsFragmentShader = component("FragmentShader", fragment_shader_t*);
		EcsClearColor = component("ClearColor", clear_color_t);
		EcsViewProjection = component("ViewProjection", view_projection_t);
		EcsWorldTransform = component("WorldTransform", world_transform_t);
		EcsScriptEngine = component("ScriptEngine", py_vm_index_t);
		EcsArgs = component("Args", args_t);
		EcsModelInstance = component("ModelInstance", model_instance_t);
		EcsModelScene = component("ModelScene", model_scene_t);

#ifndef NDEBUG

		reflect(EcsInit,
			(ecs_member_t){.name = "flags", .type = ecs_id(ecs_u32_t)},
		);

		reflect(EcsTimeStats,
			(ecs_member_t){.name = "last_update", .type = ecs_id(ecs_u64_t)},
			(ecs_member_t){.name = "fps", .type = ecs_id(ecs_u16_t)},
			(ecs_member_t){.name = "count", .type = ecs_id(ecs_u16_t)},
			(ecs_member_t){.name = "duration", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "dt", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsWindowConfig,
			(ecs_member_t){.name = "title", .type = ecs_id(ecs_string_t)},
			(ecs_member_t){.name = "size", .type = ecs_id(ecs_i32_t), .count = 2},
			(ecs_member_t){.name = "fullscreen", .type = ecs_id(ecs_bool_t)},
		);

		reflect(EcsCamera,
			(ecs_member_t){.name = "position", .type = ecs_id(ecs_f32_t), .count = 3},
			(ecs_member_t){.name = "target", .type = ecs_id(ecs_f32_t), .count = 3},
			(ecs_member_t){.name = "up", .type = ecs_id(ecs_f32_t), .count = 3},
			(ecs_member_t){.name = "fov_y", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "near_plane", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "far_plane", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsPhysicsConfig,
			(ecs_member_t){.name = "move_speed", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "max_move_speed", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "gravity_y", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "jump_speed", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsRotation,
			(ecs_member_t){.name = "x", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "y", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "z", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsPosition,
			(ecs_member_t){.name = "x", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "y", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "z", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsScale,
			(ecs_member_t){.name = "x", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "y", .type = ecs_id(ecs_f32_t)},
			(ecs_member_t){.name = "z", .type = ecs_id(ecs_f32_t)},
		);

		reflect(EcsProjection,
			(ecs_member_t){.name = "rebuild", .type = ecs_id(ecs_bool_t)},
			(ecs_member_t){.name = "value", .type = ecs_id(ecs_f32_t), .count = 16},
		);

		reflect(EcsWorldTransform,
			(ecs_member_t){.name = "value", .type = ecs_id(ecs_f32_t), .count = 16},
		);

		reflect(EcsError,
			(ecs_member_t){.name = "title", .type = ecs_id(ecs_string_t)},
			(ecs_member_t){.name = "message", .type = ecs_id(ecs_string_t)},
		);

		reflect(EcsScriptEngine,
			(ecs_member_t){.name = "vm_index", .type = ecs_id(ecs_i32_t)},
		);

		reflect(EcsArgs,
			(ecs_member_t){.name = "prefer_low_power", .type = ecs_id(ecs_bool_t)},
		);

		reflect(EcsModelInstance,
			(ecs_member_t){.name = "name", .type = ecs_id(ecs_string_t)},
		);

		reflect(EcsModelScene,
			(ecs_member_t){.name = "name", .type = ecs_id(ecs_string_t)},
		);

#endif

		create_pipeline();
	}

	ecs_scope(ecs_world(), EcsChirpEvent)
	{
		add_events();
	}
}

static void on_init_set([[maybe_unused]] ecs_iter_t *iter)
{
	const args_t *args = ecs_field(iter, args_t, 1);

	ecs_os_api_t os_api = ecs_os_api_create();
	ecs_os_set_api(&os_api);

	if (args->threads > 0)
	{
		ecs_set_threads(ecs_world(), args->threads);
		SDL_LogInfo(LOG_CATEGORY_ECS, "Using %d threads", args->threads);
	}
	else if (args->task_threads > 0)
	{
		ecs_set_task_threads(ecs_world(), args->task_threads);
		SDL_LogInfo(LOG_CATEGORY_ECS, "Using %d task threads", args->task_threads);
	}
	else
	{
		const int cores = SDL_GetNumLogicalCPUCores();
		ecs_set_threads(ecs_world(), cores);
		SDL_LogInfo(LOG_CATEGORY_ECS, "Using %d threads", cores);
	}

#ifdef FLECS_REST
	ecs_singleton_set(ecs_world(), EcsRest, {0});
#endif

#ifdef FLECS_STATS
	ECS_IMPORT(ecs_world(), FlecsStats);
#endif
}

void ecs_create()
{
	ecs_create_default();
	add_modules();

	// SDL has to initialise before we set up OS-specific stuff
	ecs_observer_init(ecs_world(), &(ecs_observer_desc_t){
		.query.terms = {
			(ecs_term_t){.id = ecs_singleton_id(EcsInit), .inout = EcsInOutNone},
			(ecs_term_t){.id = ecs_singleton_id(EcsArgs), .inout = EcsIn},
		},
		.events = {EcsOnSet},
		.callback = on_init_set,
	});
}

void ecs_destroy()
{
	ecs_destroy_default();
}

ecs_entity_t ecs_phase(const phase_t phase)
{
	const ecs_entity_t entity = phases[phase];
	SDL_assert(entity != 0);
	return entity;
}
