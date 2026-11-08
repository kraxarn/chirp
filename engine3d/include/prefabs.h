#pragma once

#include "ecs.h"

#define prefab_model(n)			\
	EcsModelInstance,			\
	sizeof(model_descriptor_t),	\
	&(model_descriptor_t){.name = (n)}

#define prefab_scene(n)		\
	EcsModelScene,			\
	sizeof(model_scene_t),	\
	&(model_scene_t){.name = (n)}
