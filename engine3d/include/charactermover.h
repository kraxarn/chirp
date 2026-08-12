#pragma once

#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "box3d/types.h"

static constexpr int character_mover_plane_capacity = 8;

typedef struct
{
	float max_push;
	bool clip_velocity;
} mover_shape_user_data_t;

typedef struct
{
	b3Pos point;
	b3ShapeId shape_id;
} plane_extra_t;

typedef struct
{
	b3WorldTransform transform;
	b3Vec3 velocity;
	b3Capsule capsule;
	b3CollisionPlane planes[character_mover_plane_capacity];
	plane_extra_t plane_extras[character_mover_plane_capacity];
	int plane_count;
	int total_iterations;
	float pogo_velocity;
	bool on_ground;
	bool sprint;

	b3ShapeId *ignore_shape_ids;
	int ignore_shape_count;
} character_mover_t;

[[nodiscard]]
character_mover_t character_mover_init(b3Pos position);

void character_mover_solve_move(character_mover_t *mover, float time_step,
	b3Vec3 forward, b3Vec3 right, b3Vec2 throttle, bool clip_velocity);

void character_mover_step(character_mover_t *mover, b3ShapeId *ignore_shapes,
	int ignore_shape_count, float time_step, bool clip_velocity);
