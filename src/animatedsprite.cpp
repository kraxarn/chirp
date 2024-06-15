#include "chirp/animatedsprite.hpp"

#include "chirp/internal/utils.hpp"

#include "raylib.h"

chirp::animated_sprite::animated_sprite(const asset<chirp::tileset> &tileset)
	: frame_time(default_frame_time),
	current_time(0.F),
	scale(default_scale),
	rotation(default_rotation),
	playing(true),
	current_frame(0),
	tileset(tileset),
	direction(default_direction)
{
}

void chirp::animated_sprite::update(const chirp::scene &/*scene*/, const float delta)
{
	if (!playing)
	{
		return;
	}

	current_time += delta;
	if (current_time < frame_time)
	{
		return;
	}

	current_time = 0.F;
	current_frame++;

	if (current_frame >= tileset->get_size())
	{
		current_frame = 0;
	}
}

void chirp::animated_sprite::draw() const
{
	tileset->draw(get_position(), current_frame, rotation, scale);
}

void chirp::animated_sprite::debug_draw(const color &color) const
{
	const auto shape = get_shape().to<int>();
	const auto r_color = internal::utils::to_r_color(color);

	DrawRectangleLines(shape.x(), shape.y(), shape.width(), shape.height(), r_color);
}

auto chirp::animated_sprite::get_scale() const -> float
{
	return scale;
}

void chirp::animated_sprite::set_scale(const float value)
{
	scale = value;
}

auto chirp::animated_sprite::get_direction() const -> chirp::direction
{
	return direction;
}

void chirp::animated_sprite::set_direction(const chirp::direction value)
{
	direction = value;
}

auto chirp::animated_sprite::get_position() const -> const vector2f &
{
	return position;
}

void chirp::animated_sprite::set_position(const vector2f &value)
{
	position = value;
}

void chirp::animated_sprite::play()
{
	playing = true;
}

void chirp::animated_sprite::pause()
{
	playing = false;
}

auto chirp::animated_sprite::get_shape() const -> rectangle<float>
{
	const auto &pos = get_position();
	const auto size = static_cast<float>(tileset->get_size()) * get_scale();
	return {pos.x(), pos.y(), size, size};
}

void chirp::animated_sprite::set_frame(const int value)
{
	current_frame = value;
}

void chirp::animated_sprite::flip_horizontal()
{
	tileset->flip_horizontal();

	set_direction(get_direction() == direction::left
		? direction::right
		: direction::left);
}
