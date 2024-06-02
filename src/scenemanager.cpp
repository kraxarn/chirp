#include "chirp/scenemanager.hpp"

void chirp::scene_manager::pop()
{
	scenes.pop();
}

auto chirp::scene_manager::peek() const -> std::shared_ptr<chirp::scene>
{
	if (scenes.empty())
	{
		return {};
	}

	return scenes.top();
}
