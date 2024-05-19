#pragma once

namespace chirp
{
	enum class direction: unsigned char
	{
		none  = 0,
		left  = 1 << 0,
		up    = 1 << 1,
		right = 1 << 2,
		down  = 1 << 3,
	};
}
