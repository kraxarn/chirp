#include "chirp/ecs.h"
#include "chirp/assets.h"
#include "chirp/ecsutils.h"
#include "chirp/input.h"
#include "chirp/logcategory.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"

#include "flecs.h"
#include "box3d/id.h"
#include "flecs/addons/module.h"
#include "flecs/private/addons.h"
#include "flecs/private/api_defines.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

static ecs_world_t *world = nullptr;

static void log_debug_info()
{
	constexpr size_t temp_len = 160;
	char temp[temp_len] = {0};

#define append(str) if(temp[0] != '\0') SDL_strlcat(temp, ", ", temp_len); SDL_strlcat(temp, str, temp_len)

#ifdef FLECS_CPP
	append("cpp");
#endif

#ifdef FLECS_MODULE
	append("module");
#endif

#ifdef FLECS_SYSTEM
	append("system");
#endif

#ifdef FLECS_PIPELINE
	append("pipeline");
#endif

#ifdef FLECS_TIMER
	append("timer");
#endif

#ifdef FLECS_META
	append("meta");
#endif

#ifdef FLECS_UNITS
	append("units");
#endif

#ifdef FLECS_JSON
	append("json");
#endif

#ifdef FLECS_DOC
	append("doc");
#endif

#ifdef FLECS_HTTP
	append("http");
#endif

#ifdef FLECS_REST
	append("rest");
#endif

#ifdef FLECS_PARSER
	append("parser");
#endif

#ifdef FLECS_QUERY_DSL
	append("query_dsl");
#endif

#ifdef FLECS_SCRIPT
	append("script");
#endif

#ifdef FLECS_STATS
	append("stats");
#endif

#ifdef FLECS_METRICS
	append("metrics");
#endif

#ifdef FLECS_ALERTS
	append("alerts");
#endif

#ifdef FLECS_LOG
	append("log");
#endif

#ifdef FLECS_JOURNAL
	append("journal");
#endif

#ifdef FLECS_APP
	append("app");
#endif

#ifdef FLECS_OS_API_IMPL
	append("os_api_impl");
#endif

#undef append

	SDL_LogDebug(LOG_CATEGORY_ECS, "Addons: %s", temp);
}

[[nodiscard]]
static ecs_entity_t module(const char *name)
{
	return ecs_module_init(ecs_world(), name, &(ecs_component_desc_t){
	});
}

static void add_modules()
{
	ecs_scope(ecs_world(), EcsChirpModule)
	{
		EcsAssets = component("Assets", assets_t);
		EcsError = component("Error", error_t);
		EcsInit = component("Init", SDL_InitFlags);
		EcsMetadata = component("Metadata", metadata_t);
		EcsPhysicsBody = component("PhysicsBody", b3BodyId);
		EcsPhysicsWorld = component("PhysicsWorld", b3WorldId);
	}

	ecs_scope(ecs_world(), EcsChirpInput)
	{
		EcsInput = component("Input", input_t);
	}

#ifndef NDEBUG

	reflect(EcsPhysicsWorld,
		(ecs_member_t){.name = "index", .type = ecs_id(ecs_u16_t)},
		(ecs_member_t){.name = "generation", .type = ecs_id(ecs_u16_t)},
	);

	reflect(EcsPhysicsBody,
		(ecs_member_t){.name = "index", .type = ecs_id(ecs_i32_t)},
		(ecs_member_t){.name = "world", .type = ecs_id(ecs_u16_t)},
		(ecs_member_t){.name = "generation", .type = ecs_id(ecs_u16_t)},
	);

#endif
}

void ecs_create_default()
{
	if (world != nullptr)
	{
		return;
	}

	log_debug_info();
	world = ecs_init();

	EcsChirp = module("Chirp");
	EcsChirpEvent = module("ChirpEvent");
	EcsChirpInput = module("ChirpInput");
	EcsChirpModule = module("ChirpModule");

	add_modules();
}

void ecs_destroy_default()
{
	ecs_fini(world);
	world = nullptr;
}

ecs_world_t *ecs_world()
{
	return world;
}

ecs_entity_t ecs_set_error(const char *title, const char *message)
{
	const ecs_entity_t entity = ecs_new(ecs_world());

	const error_t error = {
		.title = SDL_strdup(title),
		.message = SDL_strdup(message),
	};

	ecs_set_id(ecs_world(), entity, EcsError,
		sizeof(error_t), &error);

	return entity;
}
