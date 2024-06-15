#pragma once

#include <chirp/asset.hpp>
#include <chirp/color.hpp>
#include <chirp/direction.hpp>
#include <chirp/entity.hpp>
#include <chirp/rectangle.hpp>
#include <chirp/tileset.hpp>

namespace chirp
{
	class animated_sprite: public entity
	{
	public:
		explicit animated_sprite(const asset<tileset> &tileset);

		void update(const chirp::scene &scene, float delta) override;
		void draw() const override;

		void debug_draw(const color &color) const;

		[[nodiscard]]
		auto get_scale() const -> float;

		void set_scale(float value);

		[[nodiscard]]
		auto get_direction() const -> direction;

		void set_direction(direction value);

		[[nodiscard]]
		auto get_position() const -> const vector2f &;

		void set_position(const vector2f &value);

		void play();
		void pause();

		[[nodiscard]]
		auto get_shape() const -> rectangle<float>;

		void set_frame(int value);

		void flip_horizontal();

	private:
		static constexpr float default_frame_time = 1.F / 60.F * 10.F;
		static constexpr auto default_scale = 1.F;
		static constexpr auto default_rotation = 0.F;
		static constexpr auto default_direction = chirp::direction::left;

		float frame_time;
		float current_time;
		float scale;
		float rotation;

		bool playing;
		int current_frame;

		asset<chirp::tileset> tileset;
		chirp::direction direction;
		vector2f position;
	};
}
