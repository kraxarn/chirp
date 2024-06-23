#include "chirp/scenemanager.hpp"

void chirp::scene_manager::pop()
{
	scenes.pop();
}

void chirp::scene_manager::queue_pop()
{
	pop_queue_count++;
}

auto chirp::scene_manager::empty() const -> bool
{
	return scenes.empty();
}

auto chirp::scene_manager::peek() const -> const std::shared_ptr<chirp::scene> &
{
	if (scenes.empty())
	{
		log::fatal("Unable to peek, no scenes loaded");
	}

	return scenes.top();
}

auto chirp::scene_manager::get_assets() const -> const chirp::assets &
{
	return assets;
}

void chirp::scene_manager::clear_queue()
{
	while (pop_queue_count > 0)
	{
		pop();
		pop_queue_count--;
	}
}
