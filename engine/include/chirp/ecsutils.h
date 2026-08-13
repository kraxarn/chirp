#pragma once

#include "flecs.h"
#include "flecs/private/api_defines.h"

#include <SDL3/SDL_stdinc.h>

#define component(name, symbol)	\
	component_impl(name, #symbol, ECS_SIZEOF(symbol), ECS_ALIGNOF(symbol), ctor_zero, nullptr)

#ifndef NDEBUG

#define reflect(e, ...)									\
	ecs_struct_init(ecs_world(), &(ecs_struct_desc_t) {	\
		.entity = e,									\
		.members = {__VA_ARGS__},						\
	});

#define reflect_enum(e, t, ...)						\
	ecs_enum_init(ecs_world(), &(ecs_enum_desc_t){	\
		.entity = e,								\
		.constants = {__VA_ARGS__},					\
		.underlying_type = t,						\
	});

#endif

void ctor_zero(void *ptr, Sint32 count, const ecs_type_info_t *type_info);

[[nodiscard]]
ecs_id_t component_impl(const char *name, const char *symbol, ecs_size_t size,
	ecs_size_t alignment, ecs_xtor_t ctor, ecs_xtor_t dtor);
