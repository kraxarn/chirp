#include "chirp/window.hpp"

#include "raylib.h"

auto chirp::window::get_title() const -> const std::string &
{
	return title;
}

void chirp::window::set_title(const std::string &title)
{
	this->title = title;
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
