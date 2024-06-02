#pragma once

#include <chirp/gamepadaxis.hpp>
#include <chirp/gamepadbutton.hpp>
#include <chirp/keyboardkey.hpp>
#include <chirp/mousebutton.hpp>

#include <map>
#include <variant>
#include <vector>

namespace chirp
{
	using key = std::variant<gamepad_axis, gamepad_button, keyboard_key, mouse_button>;

	class keymap
	{
	public:
		keymap() = default;

		void add(const char *name, gamepad_axis key);

		void add(const char *name, gamepad_button key);

		void add(const char *name, keyboard_key key);

		void add(const char *name, mouse_button key);

		auto is_pressed(const char *name) const -> bool;

		auto is_down(const char *name) const -> bool;

	private:
		static constexpr auto default_axis_threshold = 0.75F;

		std::map<const char *, std::vector<key>> map;

		void add(const char *name, const key &key);
	};
}
