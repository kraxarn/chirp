#pragma once

#include <chirp/scene.hpp>

namespace chirp
{
	class entity
	{
	public:
		virtual ~entity() = default;

		virtual void update(const chirp::scene &scene, float delta) = 0;

		virtual void draw() = 0;
	};
}
