#include "chirp/image.hpp"

#include "raylib.h"

chirp::image::image(const std::vector<unsigned char> &data)
	: r_image(new Image())
{
	*r_image = LoadImageFromMemory(guess_file_type(data),
		data.data(), static_cast<int>(data.size()));
}

chirp::image::~image()
{
	UnloadImage(*r_image);
	delete r_image;
}

void chirp::image::flip_horizontal() const
{
	ImageFlipHorizontal(r_image);
}

auto chirp::image::data() const -> Image *
{
	return r_image;
}

auto chirp::image::guess_file_type(const std::vector<unsigned char> &data) -> const char *
{
	if (data.at(0) == 0x89
		&& data.at(1) == 0x50
		&& data.at(2) == 0x4e
		&& data.at(3) == 0x47
		&& data.at(4) == 0x0d
		&& data.at(5) == 0x0a
		&& data.at(6) == 0x1a
		&& data.at(7) == 0x0a)
	{
		return ".png";
	}

	return "";
}
