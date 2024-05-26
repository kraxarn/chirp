#include "chirp/clock.hpp"

#include "raylib.h"

auto chirp::clock::fps() -> int
{
	return GetFPS();
}

auto chirp::clock::delta() -> float
{
	return GetFrameTime();
}

auto chirp::clock::time() -> double
{
	return GetTime();
}
