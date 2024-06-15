#pragma once

#include <chirp/color.hpp>
#include <chirp/entity.hpp>
#include <chirp/vector2.hpp>

#include <string>

namespace chirp
{
	class text: public entity
	{
	public:
		text(std::string text, vector2i position, int font_size, chirp::color color);

		void update(const chirp::scene &scene, float delta) override;

		void draw() const override;

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
		chirp::color color;
	};
}
