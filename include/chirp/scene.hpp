#pragma once

#include <chirp/assets.hpp>

namespace chirp
{
	class scene
	{
	public:
		explicit scene(const assets &assets);

		virtual ~scene() = default;

		virtual void update(float delta) = 0;

		virtual void draw() = 0;
	};
}
