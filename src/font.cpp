#include "chirp/font.hpp"
#include "chirp/text.hpp"
#include "chirp/internal/utils.hpp"

#include "raylib.h"

#include <vector>

chirp::font::font(const std::vector<unsigned char> &data, const int font_size)
	: r_font(new Font())
{
	*r_font = LoadFontFromMemory(".ttf",
		data.data(), static_cast<int>(data.size()),
		font_size, font_chars, 0x100);

	SetTextureFilter(r_font->texture, TEXTURE_FILTER_POINT);
}

chirp::font::~font()
{
	UnloadFont(*r_font);
	RL_FREE(font_chars);
	delete r_font;
}

auto chirp::font::font_size() const -> int
{
	return r_font->baseSize;
}

auto chirp::font::text_size(const text &text) const -> vector2f
{
	const auto measurement = MeasureTextEx(*r_font, text.get_text().c_str(),
		static_cast<float>(text.get_font_size()), 0.F);

	return internal::utils::from_r_vector2<float>(measurement);
}

auto chirp::font::data() const -> Font *
{
	return r_font;
}
