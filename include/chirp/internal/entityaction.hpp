#pragma once

#include <chirp/entity.hpp>
#include <chirp/handle.hpp>

namespace chirp::internal
{
	using remove_action = struct remove_action
	{
		std::string name{};
	};

	using append_action = struct append_action
	{
		std::string name{};
		chirp::handle<chirp::entity> entity{};
	};

	using entity_action = std::variant<remove_action, append_action>;
}
