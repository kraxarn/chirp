#include "chirp/ecsutils.h"
#include "chirp/ecs.h"

#include "flecs.h"
#include "flecs/private/api_defines.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

void ctor_zero(void *ptr, const Sint32 count, const ecs_type_info_t *type_info)
{
	SDL_memset(ptr, 0, (size_t) count * type_info->size);
}

ecs_id_t component_impl(const char *name, const char *symbol, const ecs_size_t size,
	const ecs_size_t alignment, const ecs_xtor_t ctor, const ecs_xtor_t dtor)
{
	const ecs_entity_desc_t entity_desc = {
		.use_low_id = true,
		.name = name,
		.symbol = symbol,
	};

	const ecs_component_desc_t component_desc = {
		.entity = ecs_entity_init(ecs_world(), &entity_desc),
		.type = (ecs_type_info_t){
			.size = size,
			.alignment = alignment,
		},
	};

	const ecs_id_t component = ecs_component_init(ecs_world(), &component_desc);
	SDL_assert(component != 0);

	const ecs_type_hooks_t hooks = {
		.ctor = ctor,
		.dtor = dtor,
	};
	ecs_set_hooks_id(ecs_world(), component, &hooks);

	return component;
}
