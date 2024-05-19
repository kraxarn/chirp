#pragma once

#include <chirp/rectangle.hpp>

namespace chirp
{
	class collision
	{
	public:
		static auto check(const rectangle<float> &rec1, const rectangle<float> &rec2) -> bool;

	private:
		collision() = default;
	};
}
