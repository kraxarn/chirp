#include "chirp/scenemanager.hpp"

void chirp::scene_manager::pop()
{
	scenes.pop();
}

auto chirp::scene_manager::peek() -> std::shared_ptr<chirp::scene>
{
	return scenes.top();
}
