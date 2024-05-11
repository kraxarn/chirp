#pragma once

#include <vector>

struct Image;

namespace chirp
{
	class image
	{
	public:
		explicit image(const std::vector<unsigned char> &data);

		void flip_horizontal() const;

		[[nodiscard]]
		auto data() const -> Image *;

	private:
		Image *r_image;

		static auto guess_file_type(const std::vector<unsigned char> &data) -> const char *;
	};
}
