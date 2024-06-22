#pragma once

#include <chirp/entity.hpp>
#include <chirp/entitycontainer.hpp>
#include <chirp/vector2.hpp>

struct Camera2D;

namespace chirp
{
	class camera: public entity
	{
	public:
		camera();
		~camera() override;

		void update(const chirp::scene &scene, float delta) override;
		void draw() const override;

		[[nodiscard]]
		auto get_target() const -> vector2f;

		void set_target(const vector2f &target) const;

		[[nodiscard]]
		auto get_offset() const -> vector2f;

		void set_offset(const vector2f &offset) const;

		[[nodiscard]]
		auto get_zoom() const -> float;

		void set_zoom(float zoom) const;

		[[nodiscard]]
		auto entities() -> chirp::entity_container &;

	private:
		Camera2D *r_camera;
		chirp::entity_container entity_container;
	};
}
