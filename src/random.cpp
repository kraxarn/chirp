#include "chirp/random.hpp"

#include "raylib.h"

auto chirp::random::get(const int min, const int max) -> int
{
	return GetRandomValue(min, max);
}
