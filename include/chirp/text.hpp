#pragma once

#include "chirp/color.hpp"
#include "chirp/vector2.hpp"

#include <string>

namespace chirp
{
	class text
	{
	public:
		text(std::string text, vector2i position, int font_size, color color);

		void draw() const;

		[[nodiscard]]
		auto get_text() const -> const std::string &;

		void set_text(const std::string &text);

		[[nodiscard]]
		auto get_position() const -> const vector2i &;

		void set_position(const vector2i &value);

		[[nodiscard]]
		auto get_font_size() const -> int;

		[[nodiscard]]
		auto get_color() const -> color;

	private:
		std::string str;
		vector2i position;
		int font_size = 0;
		color color;
	};
}
