#include "chirp/color.hpp"

#include <limits>

chirp::color::color(const unsigned char red, const unsigned char green,
	const unsigned char blue, const unsigned char alpha)
	: red_val(red),
	green_val(green),
	blue_val(blue),
	alpha_val(alpha)
{
}

auto chirp::color::red() const -> unsigned char
{
	return red_val;
}

auto chirp::color::green() const -> unsigned char
{
	return green_val;
}

auto chirp::color::blue() const -> unsigned char
{
	return blue_val;
}

auto chirp::color::alpha() const -> unsigned char
{
	return alpha_val;
}
