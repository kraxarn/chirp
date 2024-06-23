#include "chirp/scene.hpp"

#include <chirp/entitycontainer.hpp>
#include <chirp/log.hpp>

/**
 * This is quite unnecessary, but I think I want it anyway for two reasons:
 * 1. Forces "assets" in the constructor to better show that's where you're supposed to load assets.
 * 2. Maybe it will load something in the future, who knows.
 */
chirp::scene::scene(const assets &/*assets*/)
	: scene_manager(nullptr),
	entity_container(new chirp::entity_container())
{
}

chirp::scene::~scene()
{
	delete entity_container;
}

void chirp::scene::update(const float delta)
{
	for (const auto &entity: entities().entities())
	{
		entity->update(*this, delta);
	}
}

void chirp::scene::draw()
{
	for (const auto &entity: entities().entities())
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

auto chirp::scene::entities() const -> chirp::entity_container &
{
	return *entity_container;
}

void chirp::scene::init(chirp::scene_manager &scene)
{
	scene_manager = &scene;
	load();
}

void chirp::scene::load()
{
	/**
	 * init is intended to initialize the base scene,
	 * while this is mostly intended to initialize
	 * inheritors, and thus left empty here.
	 *
	 * "why not just make init virtual?"
	 * just so nothing horrible happens if you
	 * forget to call the base method.
	 */
}
