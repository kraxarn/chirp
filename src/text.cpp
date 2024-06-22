#include "chirp/text.hpp"
#include "chirp/internal/monogram.hpp"
#include "chirp/internal/utils.hpp"

#include "raylib.h"

chirp::text::text(std::string text, const vector2i &position, const int font_size, const chirp::color color)
	: chirp::text(get_default_font(font_size), std::move(text), position, font_size, color)
{
}

chirp::text::text(const chirp::asset<chirp::font> &font, std::string text,
	const vector2i &position, int font_size, chirp::color color)
	: str(std::move(text)),
	position(position),
	font_size(font_size),
	color(color),
	font(font)
{
}

void chirp::text::update(const chirp::scene &/*scene*/, const float /*delta*/)
{
}

void chirp::text::draw() const
{
	const auto r_color = internal::utils::to_r_color(color);
	const auto r_position = internal::utils::to_r_vector2(position);

	SetTextLineSpacing(static_cast<int>(static_cast<float>(font_size) * 0.75F));

	DrawTextEx(*font->data(), str.c_str(), r_position,
		static_cast<float>(font_size),
		0.F, r_color);
}

auto chirp::text::get_text() const -> const std::string &
{
	return str;
}

void chirp::text::set_text(const std::string &text)
{
	str = text;
}

auto chirp::text::get_position() const -> const vector2i &
{
	return position;
}

void chirp::text::set_position(const vector2i &value)
{
	position = value;
}

auto chirp::text::get_font_size() const -> int
{
	return font_size;
}

auto chirp::text::get_color() const -> chirp::color
{
	return color;
}

auto chirp::text::measure() const -> vector2f
{
	if (font)
	{
		const auto measurement = MeasureTextEx(*font->data(), str.c_str(),
			static_cast<float>(font_size), 0.F);

		return internal::utils::from_r_vector2<float>(measurement);
	}

	const auto width = MeasureText(str.c_str(), font_size);
	return {static_cast<float>(width), 0};
}

auto chirp::text::get_default_font(const int font_size) -> asset<chirp::font>
{
	const std::vector<unsigned char> data(monogram_ttf.cbegin(), monogram_ttf.cend());
	return std::make_shared<chirp::font>(data, font_size);
}
