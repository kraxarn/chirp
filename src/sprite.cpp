#include "chirp/sprite.hpp"

chirp::sprite::sprite(const asset<chirp::texture> &texture)
	: texture(texture)
{
}

chirp::sprite::sprite(const asset<image> &image)
	: sprite(std::make_shared<chirp::texture>(image))
{
}

void chirp::sprite::update(const chirp::scene &/*scene*/, const float /*delta*/)
{
}

void chirp::sprite::draw() const
{
	texture->draw(get_position());
}

auto chirp::sprite::get_size() const -> vector2i
{
	return texture->size();
}

auto chirp::sprite::get_position() const -> const vector2f &
{
	return position;
}

void chirp::sprite::set_position(const vector2f &value)
{
	position = value;
}
