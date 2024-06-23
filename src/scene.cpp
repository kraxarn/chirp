#include "chirp/scene.hpp"

#include <chirp/entity.hpp>
#include <chirp/entitycontainer.hpp>
#include <chirp/log.hpp>
#include <chirp/scenemanager.hpp>

chirp::scene::scene()
	: chirp::entity_container(),
	scene_manager(nullptr)
{
}

void chirp::scene::update(const float delta)
{
	for (const auto &entity: entities())
	{
		entity->update(*this, delta);
	}

	clear_queue();
}

void chirp::scene::draw()
{
	for (const auto &entity: entities())
	{
		entity->draw();
	}
}

auto chirp::scene::scenes() const -> chirp::scene_manager &
{
	if (scene_manager == nullptr)
	{
		log::fatal("Attemped to fetch scene manager before initialization");
		throw std::runtime_error("scene_manager == nullptr");
	}

	return *scene_manager;
}

auto chirp::scene::window() const -> const chirp::window &
{
	return window_manager;
}

auto chirp::scene::assets() const -> const chirp::assets &
{
	return scenes().get_assets();
}

void chirp::scene::init(chirp::scene_manager &scene)
{
	scene_manager = &scene;
}
