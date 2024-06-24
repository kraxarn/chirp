#pragma once

#include <chirp/scene.hpp>

namespace chirp::internal
{
	using pop_action = struct pop_action
	{
	};

	using push_action = struct push_action
	{
		chirp::scene *scene{};
	};

	using scene_action = std::variant<pop_action, push_action>;
}
