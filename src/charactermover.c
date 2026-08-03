#include "charactermover.h"
#include "camera.h"
#include "ecs.h"
#include "ecs/components.h"
#include "vector.h"

#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/collision.h"
#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "box3d/types.h"

#include <SDL3/SDL_assert.h>

#include <float.h>
#include <stddef.h>

/*
 * I wish I knew how this works, but this is
 * very copy-pasted from the official example
 */

static constexpr float jump_speed = 5.F;
static constexpr float max_speed = 6.F;
static constexpr float min_speed = 0.01F;
static constexpr float stop_speed = 1.F;
static constexpr float accelerate = 30.F;
static constexpr float friction = 4.F;
static constexpr float gravity = 15.F;

character_mover_t character_mover_init(const b3Pos position)
{
	return (character_mover_t){
		.transform = (b3Transform){
			.p = position,
			.q = b3Quat_identity,
		},
		.velocity = b3Vec3_zero,
		.capsule = (b3Capsule){
			.center1 = (b3Vec3){.x = 0.F, .y = -0.5F, .z = 0.F},
			.center2 = (b3Vec3){.x = 0.F, .y = 0.5F, .z = 0.F},
			.radius = 0.3F,
		},
		.plane_count = 0,
		.total_iterations = 0,
		.pogo_velocity = 0.F,
		.on_ground = false,
		.sprint = false,

		.ignore_shape_ids = nullptr,
		.ignore_shape_count = 0,
	};
}

[[nodiscard]]
static bool is_mover_filter(const b3ShapeId shape_id, void *context)
{
	const character_mover_t *mover = context;

	for (size_t i = 0; i < mover->ignore_shape_count; i++)
	{
		if (B3_ID_EQUALS(shape_id, mover->ignore_shape_ids[i]))
		{
			return false;
		}
	}

	return true;
}

static bool plane_result(const b3ShapeId shape_id, const b3PlaneResult *plane_results,
	const int plane_count, void *context)
{
	if (!is_mover_filter(shape_id, context))
	{
		return true;
	}

	character_mover_t *mover = context;
	float max_push = FLT_MAX;
	bool clip_velocity = true;

	const mover_shape_user_data_t *user_data = b3Shape_GetUserData(shape_id);
	if (user_data != nullptr)
	{
		max_push = user_data->max_push;
		clip_velocity = user_data->clip_velocity;
	}

	for (int i = 0; i < plane_count && mover->plane_count < character_mover_plane_capacity; i++)
	{
		SDL_assert(b3IsValidPlane(plane_results[i].plane));

		mover->planes[mover->plane_count] = (b3CollisionPlane){
			.plane = plane_results[i].plane,
			.pushLimit = max_push,
			.push = 0.F,
			.clipVelocity = clip_velocity,
		};
		mover->plane_extras[mover->plane_count] = (plane_extra_t){
			.point = b3OffsetPos(mover->transform.p, plane_results[i].point),
			.shape_id = shape_id,
		};
		mover->plane_count += 1;
	}

	return true;
}

void character_mover_solve_move(character_mover_t *mover, float time_step, b3Vec3 forward, b3Vec3 right,
	b3Vec2 throttle, bool clip_velocity)
{
	float speed = b3Length(mover->velocity);
	if (speed < min_speed)
	{
		mover->velocity.x = 0.F;
		mover->velocity.z = 0.F;
	}
	else
	{
		const float control = speed < stop_speed ? stop_speed : speed;

		const float drop = control * friction * time_step;
		const float new_speed = b3MaxFloat(0.F, speed - drop);
		const float ratio = new_speed / speed;
		mover->velocity.x *= ratio;
		mover->velocity.z *= ratio;
	}

	const float mover_max_speed = (int) mover->sprint
		? max_speed * 1.5F
		: max_speed;

	b3Vec3 desired_velocity = b3Add(
		b3MulSV(mover_max_speed * throttle.x, forward),
		b3MulSV(mover_max_speed * throttle.y, right)
	);
	float desired_speed = 0.F;
	const b3Vec3 desired_direction = b3GetLengthAndNormalize(&desired_speed, desired_velocity);

	if (desired_speed > mover_max_speed)
	{
		desired_velocity = b3MulSV(mover_max_speed / desired_speed, desired_velocity);
		desired_speed = mover_max_speed;
	}

	if (mover->on_ground)
	{
		mover->velocity.y = 0.F;
	}

	const float current_speed = b3Dot(mover->velocity, desired_direction);
	const float add_speed = desired_speed - current_speed;
	if (add_speed > 0.F)
	{
		float accel_speed = accelerate * mover_max_speed * time_step;
		if (accel_speed > add_speed)
		{
			accel_speed = add_speed;
		}

		mover->velocity = b3Add(mover->velocity, b3MulSV(accel_speed, desired_direction));
	}

	mover->velocity.y -= gravity * time_step;

	// TODO: Do we want to do it like this?
	const b3WorldId world_id = *(b3WorldId*) ecs_get_id(ecs_world(),
		ecs_singleton(EcsPhysicsWorld));

	const float pogo_rest_length = mover->capsule.radius * 3.F;
	const float ray_length = pogo_rest_length * mover->capsule.radius;
	const b3Pos ray_origin = b3TransformWorldPoint(mover->transform, mover->capsule.center1);
	const b3Vec3 ray_translation = b3MulSV(-ray_length, b3Vec3_axisY);
	const b3QueryFilter skip_team_filter = {
		.categoryBits = 1,
		.maskBits = ~2U,
		.name = "pogo",
	};
	const b3RayResult ray_result = b3World_CastRayClosest(world_id,
		ray_origin, ray_translation, skip_team_filter);

	const bool suppress_pogo = mover->velocity.y > 0.F;

	if (!ray_result.hit || suppress_pogo)
	{
		mover->on_ground = false;
		mover->pogo_velocity = 0.F;
	}
	else
	{
		mover->on_ground = true;
		const float pogo_current_length = ray_result.fraction * ray_length;

		constexpr float zeta = 0.7F;
		constexpr float hertz = 4.F;
		const float omega = 2.F * B3_PI * hertz;
		const float omega_h = omega * time_step;

		mover->pogo_velocity = (mover->pogo_velocity - (omega * omega_h * (pogo_current_length - pogo_rest_length)))
			/ (1.F + (2.F * zeta * omega_h) + (omega_h * omega_h));
	}

	const b3Pos start_position = mover->transform.p;
	const b3Pos target = b3Add(
		b3Add(mover->transform.p, b3MulSV(time_step, mover->velocity)),
		b3MulSV(time_step * mover->pogo_velocity, b3Vec3_axisY)
	);

	const b3QueryFilter mover_filter = {
		.categoryBits = 1,
		.maskBits = ~0U,
		.id = 1,
		.name = "mover_collide",
	};

	const b3QueryFilter cast_filter = {
		.categoryBits = 1,
		.maskBits = ~2U,
		.id = 1,
		.name = "mover_cast",
	};

	mover->total_iterations = 0;
	const float tolerance = 0.01F;

	for (int i = 0; i < 5; i++)
	{
		mover->plane_count = 0;

		const b3Capsule temp = {
			.center1 = mover->capsule.center1,
			.center2 = mover->capsule.center2,
			.radius = mover->capsule.radius,
		};

		b3World_CollideMover(world_id, mover->transform.p,
			&temp, mover_filter, plane_result, mover);

		const b3Vec3 target_delta = b3Sub(target, mover->transform.p);
		const b3PlaneSolverResult result = b3SolvePlanes(target_delta, mover->planes, mover->plane_count);

		mover->total_iterations += result.iterationCount;

		b3Vec3 delta = result.delta;

		float fraction = b3World_CastMover(world_id, mover->transform.p,
			&temp, delta, cast_filter, is_mover_filter, mover);

		delta = b3MulSV(fraction, delta);
		mover->transform.p = b3Add(mover->transform.p, delta);

		if (b3LengthSquared(delta) < tolerance * tolerance)
		{
			break;
		}
	}

	for (int i = 0; i < mover->plane_count; ++i)
	{
		const b3BodyId body_id = b3Shape_GetBody(mover->plane_extras[i].shape_id);
		const b3BodyType body_type = b3Body_GetType(body_id);
		if (body_type != b3_dynamicBody)
		{
			continue;
		}

		const b3Pos point = mover->plane_extras[i].point;
		const b3Vec3 normal = b3Neg(mover->planes[i].plane.normal);

		constexpr float inv_mass_a = 0.F;
		const float inv_mass_b = b3Body_GetInverseMass(body_id);
		const b3Matrix3 inv_ib = b3Body_GetWorldInverseRotationalInertia(body_id);

		const b3Pos p_b = b3Body_GetWorldCenter(body_id);
		const b3Vec3 r_b = b3SubPos(point, p_b);

		const b3Vec3 rn_b = b3Cross(r_b, normal);
		const float k_normal = inv_mass_a + inv_mass_b + b3Dot(rn_b, b3MulMV(inv_ib, rn_b));
		const float normal_mass = k_normal > 0.F ? 1.F / k_normal : 0.F;

		const b3Vec3 v_b = b3Body_GetLinearVelocity(body_id);
		const b3Vec3 omega_b = b3Body_GetAngularVelocity(body_id);
		const b3Vec3 vr_b = b3Add(v_b, b3Cross(omega_b, r_b));
		const float vn = b3Dot(b3Sub(vr_b, mover->velocity), normal);
		const float impulse = b3MaxFloat(-normal_mass * vn, 0.0f);

		const b3Vec3 p = b3MulSV(impulse, normal);
		mover->velocity = b3MulSub(mover->velocity, inv_mass_a, p);

		b3Body_ApplyLinearImpulse(body_id, p, point, true);
	}

	if (clip_velocity)
	{
		mover->velocity = b3ClipVector(mover->velocity, mover->planes, mover->plane_count);
	}
	else if (time_step > 0.F)
	{
		mover->velocity = b3MulSV(1.F / time_step, b3Sub(mover->transform.p, start_position));
	}
}

void character_mover_step(character_mover_t *mover, b3ShapeId *ignore_shapes,
	const int ignore_shape_count, const float time_step, const bool clip_velocity)
{
	mover->ignore_shape_ids = ignore_shapes;
	mover->ignore_shape_count = ignore_shape_count;

	// TODO: Do we want to do it like this?
	const camera_t *camera = ecs_get_id(ecs_world(),ecs_singleton(EcsCamera));

	const b3Vec2 throttle = {.x = 0.F, .y = 0.F};
	b3Vec3 forward = vector3f_to_b3vec3(vector3f_invert(camera_forward(camera)));
	const b3Vec3 right = vector3f_to_b3vec3(camera_right(camera));

	forward.y = 0.F;
	forward = b3Normalize(forward);

	character_mover_solve_move(mover, time_step,
		forward, right, throttle, clip_velocity);

	mover->ignore_shape_ids = nullptr;
	mover->ignore_shape_count = 0;
}
