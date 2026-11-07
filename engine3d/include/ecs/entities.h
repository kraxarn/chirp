#pragma once

#include "ecs.h"

extern ecs_entity_t EcsPhaseUpdateBegin;   // TODO
extern ecs_entity_t EcsPhaseUpdate;        // Update game logic
extern ecs_entity_t EcsPhaseUpdateEnd;     // TODO
extern ecs_entity_t EcsPhasePhysicsUpdate; // Update/tick physics engine
extern ecs_entity_t EcsPhasePhysicsSync;   // Sync physics engine changes to entities
extern ecs_entity_t EcsPhaseRenderBegin;   // Begin rendering
extern ecs_entity_t EcsPhaseRender;        // Render all renderable entities
extern ecs_entity_t EcsPhaseRenderEnd;     // End rendering and submit to GPU

extern ecs_id_t EcsInstanceOf;
