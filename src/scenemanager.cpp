#include "chirp/scenemanager.hpp"

void chirp::scene_manager::pop()
{
	scenes.pop();
}

auto chirp::scene_manager::peek() -> scene *
{
	return scenes.top().get();
}
