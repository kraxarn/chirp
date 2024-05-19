#include "chirp/colors.hpp"

#include <limits>

auto chirp::colors::min() -> unsigned char
{
	return std::numeric_limits<unsigned char>::min();
}

auto chirp::colors::max() -> unsigned char
{
	return std::numeric_limits<unsigned char>::max();
}

auto chirp::colors::black() -> color
{
	return {min(), min(), min()};
}

auto chirp::colors::white() -> color
{
	return {max(), max(), max()};
}

auto chirp::colors::red() -> color
{
	return {max(), min(), min()};
}

auto chirp::colors::green() -> color
{
	return {min(), max(), min()};
}

auto chirp::colors::blue() -> color
{
	return {min(), min(), max()};
}
