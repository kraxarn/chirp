#pragma once

#include "ecs.h"

#define prefab_model(n)			\
	EcsModelInstance,			\
	sizeof(model_instance_t),	\
	&(model_instance_t){.name = (n)}

#define prefab_scene(n)		\
	EcsModelScene,			\
	sizeof(model_scene_t),	\
	&(model_scene_t){.name = (n)}
