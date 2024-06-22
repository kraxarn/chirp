#pragma once

#include <chirp/asset.hpp>
#include <chirp/entity.hpp>
#include <chirp/music.hpp>

namespace chirp
{
	class jukebox: public entity
	{
	public:
		jukebox() = default;

		void update(const chirp::scene &scene, float delta) override;
		void draw() const override;

		void insert(const asset<music> &music);

		[[nodiscard]]
		auto empty() const -> bool;

		void set_volume(float value);

		void play() const;
		void stop() const;

		[[nodiscard]]
		auto name() const -> const std::string &;

	private:
		float volume = 1.F;

		asset<music> current;
	};
}
