#include "chirp/rectangle.hpp"
#include "chirp/internal/utils.hpp"

#include "raylib.h"

void chirp::draw_filled(const chirp::rectangle<int> &rectangle, const chirp::color &color)
{
	const auto r_color = internal::utils::to_r_color(color);
	DrawRectangle(rectangle.x(), rectangle.y(), rectangle.width(), rectangle.height(), r_color);
}

void chirp::draw_outline(const chirp::rectangle<int> &rectangle, const chirp::color &color)
{
	const auto r_color = internal::utils::to_r_color(color);
	DrawRectangleLines(rectangle.x(), rectangle.y(), rectangle.width(), rectangle.height(), r_color);
}
