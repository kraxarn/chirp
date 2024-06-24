#include "chirp/scenemanager.hpp"

void chirp::scene_manager::queue_push(chirp::scene *scene)
{
	queue.emplace(internal::push_action{
		.scene = scene,
	});
}

void chirp::scene_manager::queue_pop()
{
	queue.emplace(internal::pop_action{});
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
	while (!queue.empty())
	{
		const auto &action = queue.front();

		if (std::holds_alternative<internal::pop_action>(action))
		{
			scenes.pop();
		}
		else if (const auto *push_action = std::get_if<internal::push_action>(&action))
		{
			push(push_action->scene);
		}

		queue.pop();
	}
}
