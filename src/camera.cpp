#include "chirp/camera.hpp"
#include "chirp/internal/utils.hpp"

#include "raylib.h"

chirp::camera::camera()
	: entity_container(),
	r_camera(new Camera2D())
{
	r_camera->zoom = 1.F;
}

chirp::camera::~camera()
{
	delete r_camera;
}

void chirp::camera::update(const chirp::scene &scene, const float delta)
{
	for (const auto &entity: entities())
	{
		entity->update(scene, delta);
	}

	clear_queue();
}

void chirp::camera::draw() const
{
	BeginMode2D(*r_camera);

	for (const auto &entity: entities())
	{
		entity->draw();
	}

	EndMode2D();
}

auto chirp::camera::get_target() const -> vector2f
{
	return internal::utils::from_r_vector2<float>(r_camera->target);
}

void chirp::camera::set_target(const vector2f &target) const
{
	r_camera->target = internal::utils::to_r_vector2(target);
}

auto chirp::camera::get_offset() const -> vector2f
{
	return internal::utils::from_r_vector2<float>(r_camera->offset);
}

void chirp::camera::set_offset(const vector2f &offset) const
{
	r_camera->offset = internal::utils::to_r_vector2(offset);
}

auto chirp::camera::get_zoom() const -> float
{
	return r_camera->zoom;
}

void chirp::camera::set_zoom(const float zoom) const
{
	r_camera->zoom = zoom;
}
