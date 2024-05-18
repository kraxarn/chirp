#include "chirp/tileset.hpp"

#include "raylib.h"

chirp::tileset::tileset(const std::vector<unsigned char> &data)
	: image(data),
	texture(image)
{
	const auto height = texture.size().y();
	size = {height, height};
}

auto chirp::tileset::at(const int index) const -> chirp::image
{
	const auto size_f = size.to<float>();

	const Rectangle rect{
		.x = static_cast<float>(size.x() * index),
		.y = 0.F,
		.width = size_f.x(),
		.height = size_f.y(),
	};

	int size;
	const auto r_image = ImageFromImage(*image.data(), rect);

	auto *data = ExportImageToMemory(r_image, ".png", &size);
	const std::vector buffer(data, data + size);

	RL_FREE(data);
	return chirp::image(buffer);
}

void chirp::tileset::draw(const vector2f &position, const int index,
	const float rotation, const float scale) const
{
	texture.draw(position, size, size.x() * index, rotation, scale);
}

void chirp::tileset::flip_horizontal()
{
	image.flip_horizontal();
	texture = chirp::texture(image);
}

auto chirp::tileset::get_size() const -> int
{
	return size.x();
}
