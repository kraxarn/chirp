#include "chirp/font.hpp"

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

auto chirp::font::data() const -> Font *
{
	return r_font;
}
