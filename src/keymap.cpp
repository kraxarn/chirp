#include "chirp/keymap.hpp"

#include "raylib.h"

void chirp::keymap::add(const char *name, gamepad_axis key)
{
	add(name, chirp::key(key));
}

void chirp::keymap::add(const char *name, gamepad_button key)
{
	add(name, chirp::key(key));
}

void chirp::keymap::add(const char *name, keyboard_key key)
{
	add(name, chirp::key(key));
}

void chirp::keymap::add(const char *name, mouse_button key)
{
	add(name, chirp::key(key));
}

void chirp::keymap::add(const char *name, const key &key)
{
	if (!map.contains(name))
	{
		map[name] = std::vector<chirp::key>();
	}

	map[name].push_back(key);
}

auto chirp::keymap::is_pressed(const char *name) const -> bool
{
	if (!map.contains(name))
	{
		return false;
	}

	return std::ranges::any_of(map.at(name), [](const key &key) -> bool
	{
		if (std::holds_alternative<keyboard_key>(key))
		{
			const auto kb_key = static_cast<int>(std::get<keyboard_key>(key));
			if (IsKeyPressed(kb_key))
			{
				return true;
			}
		}

		if (std::holds_alternative<mouse_button>(key))
		{
			const auto m_button = static_cast<int>(std::get<mouse_button>(key));
			if (IsMouseButtonPressed(m_button))
			{
				return true;
			}
		}

		if (!IsGamepadAvailable(0))
		{
			return false;
		}

		// gamepad_axis doesn't make sense for "is pressed"

		if (std::holds_alternative<gamepad_button>(key))
		{
			const auto gp_button = static_cast<int>(std::get<gamepad_button>(key));
			if (IsGamepadButtonPressed(0, gp_button))
			{
				return true;
			}
		}

		return false;
	});
}

auto chirp::keymap::is_down(const char *name) const -> bool
{
	if (!map.contains(name))
	{
		return false;
	}

	return std::ranges::any_of(map.at(name), [](const key &key) -> bool
	{
		if (std::holds_alternative<keyboard_key>(key))
		{
			const auto kb_key = static_cast<int>(std::get<keyboard_key>(key));
			if (IsKeyDown(kb_key))
			{
				return true;
			}
		}

		if (std::holds_alternative<mouse_button>(key))
		{
			const auto m_button = static_cast<int>(std::get<mouse_button>(key));
			if (IsMouseButtonDown(m_button))
			{
				return true;
			}
		}

		if (!IsGamepadAvailable(0))
		{
			return false;
		}

		if (std::holds_alternative<gamepad_axis>(key))
		{
			const auto axis = static_cast<int>(std::get<gamepad_axis>(key));
			const auto movement = GetGamepadAxisMovement(0, axis);
			if (movement >= default_axis_threshold)
			{
				return true;
			}
		}

		if (std::holds_alternative<gamepad_button>(key))
		{
			const auto gp_button = static_cast<int>(std::get<gamepad_button>(key));
			if (IsGamepadButtonDown(0, gp_button))
			{
				return true;
			}
		}

		return false;
	});
}
