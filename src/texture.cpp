#include "chirp/texture.hpp"
#include "chirp/internal/utils.hpp"

#include "raylib.h"

chirp::texture::texture(const asset<image> &image)
	: r_texture(new Texture2D())
{
	*r_texture = LoadTextureFromImage(*image->data());
}

chirp::texture::~texture()
{
	// UnloadTexture(r_texture);
	// delete r_texture;
}

void chirp::texture::draw(const vector2f &position) const
{
	const auto pos = position.to<int>();
	DrawTexture(*r_texture, pos.x(), pos.y(), WHITE);
}

void chirp::texture::draw(const vector2f &position, const vector2i &size, const int offset) const
{
	const auto size_f = size.to<float>();

	const Rectangle rect{
		.x = static_cast<float>(offset),
		.y = 0.F,
		.width = size_f.x(),
		.height = size_f.y(),
	};

	const auto r_pos = internal::utils::to_r_vector2(position);

	DrawTextureRec(*r_texture, rect, r_pos, WHITE);
}

void chirp::texture::draw(const vector2f &position, const vector2i &size,
	const int offset, const float rotation, const float scale) const
{
	const auto size_f = size.to<float>();

	const Rectangle rect{
		.x = static_cast<float>(offset),
		.y = 0.F,
		.width = size_f.x(),
		.height = size_f.y(),
	};

	const Rectangle dest{
		.x = position.x(),
		.y = position.y(),
		.width = size_f.x() * scale,
		.height = size_f.y() * scale,
	};

	constexpr Vector2 orig{
		0, 0,
	};

	DrawTexturePro(*r_texture, rect, dest, orig, rotation, WHITE);
}

auto chirp::texture::size() const -> vector2i
{
	return {r_texture->width, r_texture->height};
}
