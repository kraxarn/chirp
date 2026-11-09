#include "assethelper.h"
#include "ecs.h"
#include "model.h"
#include "ecs/components.h"
#include "ecs/entities.h"
#include "ecs/tags.h"

#include "flecs.h"
#include "chirp/assets.h"
#include "chirp/ecs.h"
#include "chirp/logcategory.h"
#include "chirp/matrix.h"
#include "chirp/modelinfo.h"
#include "chirp/vector.h"
#include "chirp/ecs/components.h"
#include "chirp/ecs/modules.h"
#include "flecs/addons/system.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

[[nodiscard]]
static ecs_entity_t models_entity(ecs_world_t *world)
{
	const ecs_entity_desc_t entity_desc = {
		.name = "Model",
	};
	return ecs_entity_init(world, &entity_desc);
}

static Uint16 fix_entity_name(const char *name)
{
	Uint16 changes = 0;

	char *str;
	while ((str = SDL_strchr(name, '.')) != nullptr)
	{
		*str = '_';
		changes++;
	}

	return changes;
}

static ecs_entity_t load_model(ecs_world_t *world, SDL_GPUDevice *device,
	const assets_t *assets, const char *name)
{
	SDL_LogInfo(LOG_CATEGORY_ECS, "Loading model: '%s'", name);

	model_t model;
	if (!assets_load_model(assets, device, name, &model))
	{
		SDL_LogError(LOG_CATEGORY_MODEL, "Failed to load model '%s': %s",
			name, SDL_GetError());
		return 0;
	}

	const ecs_entity_t entity = ecs_entity_init(world, &(ecs_entity_desc_t){
		.name = name,
	});
	ecs_add_pair(world, entity, EcsChildOf, models_entity(world));

	ecs_set_id(world, entity, EcsModel,
		sizeof(model_t), &model);

	for (size_t i = 0; i < model.info.node_count; i++)
	{
		char *node_name = SDL_strdup(model_node_name(&model.info, i));
		if (fix_entity_name(node_name) > 0)
		{
			SDL_LogWarn(LOG_CATEGORY_MODEL, "Renamed invalid entity name '%s' to '%s' in '%s'",
				model_node_name(&model.info, i), node_name, name);
		}
		const ecs_entity_t node = ecs_entity_init(world, &(ecs_entity_desc_t){
			.name = node_name,
		});
		SDL_free(node_name);

		ecs_add_pair(world, node, EcsChildOf, entity);

		const vector3f_t position = model_node_translation(&model.info, i);
		ecs_set_id(world, node, EcsPosition,
			sizeof(vector3f_t), &position);

		const matrix4x4_t world_transform = model_node_world_transform(&model.info, i);
		ecs_set_id(world, node, EcsWorldTransform,
			sizeof(matrix4x4_t), &world_transform);
	}

	return entity;
}

static void create_instance(ecs_world_t *world,
	const ecs_entity_t entity, const ecs_entity_t model)
{
	SDL_LogDebug(LOG_CATEGORY_ECS, "Creating new instance of model '%s'",
		ecs_get_name(world, model));

	const ecs_entity_t instance = ecs_entity_init(world, &(ecs_entity_desc_t){
		// Should be fine to copy name, they probably have different paths
		.name = ecs_get_name(world, model),
		.parent = entity,
	});

	ecs_add_id(world, instance,
		ecs_pair(EcsInstanceOf, model));

	ecs_iter_t iter = ecs_children(world, model);
	while (ecs_children_next(&iter))
	{
		for (Sint32 i = 0; i < iter.count; i++)
		{
			const ecs_entity_t child = iter.entities[i];

			const ecs_entity_t node = ecs_entity_init(world, &(ecs_entity_desc_t){
				.name = ecs_get_name(world, child),
				.parent = instance,
			});

			const projection_t projection = {.rebuild = true};
			ecs_set_id(world, node, EcsProjection,
				sizeof(projection_t), &projection);

			ecs_add_pair(world, node, EcsInstanceOf, child);
		}
	}
	ecs_iter_fini(&iter);
}

static void instance_model(ecs_iter_t *iter)
{
	// Only do one at a time for now, just to avoid
	// handling for loading the same model multiple times
	// (and to balance load or some other excuse)

	SDL_assert(iter->count >= 1);

	const char *name = ecs_field(iter, model_descriptor_t, 0)->name;

	const ecs_entity_t entity = iter->entities[0];
	SDL_assert(entity != 0);

	const ecs_entity_t parent = models_entity(iter->stage);
	const ecs_entity_t model = ecs_lookup_child(iter->stage, parent, name);
	if (model == 0)
	{
		SDL_GPUDevice *device = *ecs_field(iter, SDL_GPUDevice*, 1);
		const assets_t *assets = ecs_field(iter, assets_t, 2);

		if (load_model(iter->stage, device, assets, name) == 0)
		{
			// Don't try to load indefinitely
			ecs_remove_id(iter->stage, entity, EcsModelInstance);
			ecs_remove_id(iter->stage, entity, EcsModelScene);
		}

		// Deferred, I don't really like this, but it works
		return;
	}

	if (ecs_has_id(iter->stage, entity, EcsModelInstance))
	{
		create_instance(iter->stage, entity, model);
		ecs_remove_id(iter->stage, entity, EcsModelInstance);
	}
	else if (ecs_has_id(iter->stage, entity, EcsModelScene))
	{
		// TODO: We might want to keep the entity?
		ecs_add_id(iter->stage, model, EcsScene);
		ecs_delete(iter->stage, entity);
	}
}

void ecs_add_models()
{
	ecs_scope(ecs_world(), EcsChirpSystem)
	{
		const ecs_entity_t entity_load_models = ecs_entity_init(ecs_world(), &(ecs_entity_desc_t){
			.name = "LoadModels",
		});
		ecs_add_id(ecs_world(), entity_load_models,ecs_phase(RenderBegin));

		ecs_system_init(ecs_world(), &(ecs_system_desc_t){
			.entity = entity_load_models,
			.query.terms = {
				(ecs_term_t){.id = EcsModelInstance, .oper = EcsOr, .inout = EcsInOut},
				(ecs_term_t){.id = EcsModelScene, .inout = EcsInOut},
				(ecs_term_t){.id = ecs_singleton_id(EcsGpuDevice), .inout = EcsInOut},
				(ecs_term_t){.id = ecs_singleton_id(EcsAssets), .inout = EcsIn},
			},
			.callback = instance_model,
		});
	}
}
