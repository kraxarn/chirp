#include "chirp/window.hpp"

#include "raylib.h"

/*
 * this could be a static class, but passing around
 * an instance feels better and is more consistent
 */

void chirp::window::set_title(const std::string &title) const
{
	SetWindowTitle(title.c_str());
}

auto chirp::window::get_size() const -> chirp::vector2i
{
	return {
		GetScreenWidth(),
		GetScreenHeight(),
	};
}

void chirp::window::set_size(const chirp::vector2i &size) const
{
	SetWindowSize(size.x(), size.y());
}

auto chirp::window::is_resized() const -> bool
{
	return IsWindowResized();
}
