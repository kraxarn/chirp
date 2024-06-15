#pragma once

#include <vector>

struct Font;

namespace chirp
{
	class font
	{
	public:
		explicit font(const std::vector<unsigned char> &data, int font_size);
		~font();

		[[nodiscard]]
		auto font_size() const -> int;

		[[nodiscard]]
		auto data() const -> Font *;

	private:
		Font *r_font;
		int *font_chars = nullptr;
	};
}
