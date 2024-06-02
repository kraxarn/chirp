#pragma once

#include "chirp/text.hpp"

#include <vector>

struct Font;

namespace chirp
{
	class font
	{
	public:
		explicit font(const std::vector<unsigned char> &data, int font_size);
		~font();

		void draw_text(const chirp::text &text) const;

		[[nodiscard]]
		auto text_size(const chirp::text &text) const -> vector2f;

		[[nodiscard]]
		auto font_size() const -> int;

	private:
		Font *r_font;
		int *font_chars = nullptr;
	};
}
