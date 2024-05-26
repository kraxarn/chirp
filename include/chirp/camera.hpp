#pragma once

#include <chirp/vector2.hpp>

struct Camera2D;

namespace chirp
{
	class camera
	{
	public:
		camera();
		~camera();

		void begin() const;
		void end() const;

		[[nodiscard]]
		auto get_target() const -> vector2f;

		void set_target(const vector2f &target) const;

		[[nodiscard]]
		auto get_offset() const -> vector2f;

		void set_offset(const vector2f &offset) const;

		[[nodiscard]]
		auto get_zoom() const -> float;

		void set_zoom(float zoom) const;

	private:
		Camera2D *r_camera;
	};
}
