#pragma once

namespace chirp
{
	class scene;

	class entity
	{
	public:
		virtual ~entity() = default;

		virtual void update(const chirp::scene &scene, float delta) = 0;

		virtual void draw() const = 0;
	};
}
