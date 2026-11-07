#include "ecs/entities.h"
#include "ecs.h"

ecs_entity_t EcsPhaseUpdateBegin = 0;
ecs_entity_t EcsPhaseUpdate = 0;
ecs_entity_t EcsPhaseUpdateEnd = 0;
ecs_entity_t EcsPhasePhysicsUpdate = 0;
ecs_entity_t EcsPhasePhysicsSync = 0;
ecs_entity_t EcsPhaseRenderBegin = 0;
ecs_entity_t EcsPhaseRender = 0;
ecs_entity_t EcsPhaseRenderEnd = 0;

ecs_entity_t EcsInstanceOf = 0;
