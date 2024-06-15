#pragma once

#include <chirp/asset.hpp>
#include <chirp/color.hpp>
#include <chirp/entity.hpp>
#include <chirp/font.hpp>
#include <chirp/vector2.hpp>

#include <string>

namespace chirp
{
	class text: public entity
	{
	public:
		/**
		 * Create text with default font
		 */
		explicit text(std::string text, const vector2i &position, int font_size, chirp::color color);

		/**
		 * Create text with specified font
		 */
		explicit text(const chirp::asset<chirp::font> &font, std::string text,
			const vector2i &position, int font_size, chirp::color color);

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

		[[nodiscard]]
		auto measure() const -> vector2f;

	private:
		std::string str;
		vector2i position;
		int font_size = 0;
		chirp::color color;
		chirp::asset<chirp::font> font;
	};
}
