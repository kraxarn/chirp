#include "chirp/internal/utils.hpp"

auto chirp::internal::utils::to_r_color(const color &color) -> Color
{
	return Color{
		.r = color.red(),
		.g = color.green(),
		.b = color.blue(),
		.a = color.alpha(),
	};
}
