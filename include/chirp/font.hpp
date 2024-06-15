#pragma once

#include <chirp/vector2.hpp>

#include <vector>

struct Font;

namespace chirp
{
	class text;

	class font
	{
	public:
		explicit font(const std::vector<unsigned char> &data, int font_size);
		~font();

		[[nodiscard]]
		auto text_size(const chirp::text &text) const -> vector2f;

		[[nodiscard]]
		auto font_size() const -> int;

		[[nodiscard]]
		auto data() const -> Font *;

	private:
		Font *r_font;
		int *font_chars = nullptr;
	};
}
