#include "monitor.hpp"

#include "raylib.h"

auto chirp::monitor::count() -> int
{
	return GetMonitorCount();
}

auto chirp::monitor::current() -> int
{
	return GetCurrentMonitor();
}

auto chirp::monitor::position(int index) -> vector2f
{
	const auto position = GetMonitorPosition(index);
	return {position.x, position.y};
}

auto chirp::monitor::size(int index) -> vector2i
{
	return {
		GetMonitorWidth(index),
		GetMonitorHeight(index),
	};
}

auto chirp::monitor::refresh_rate(int index) -> int
{
	return GetMonitorRefreshRate(index);
}
