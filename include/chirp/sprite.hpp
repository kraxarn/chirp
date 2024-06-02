#pragma once

#include <chirp/asset.hpp>
#include <chirp/texture.hpp>
#include <chirp/vector2.hpp>

namespace chirp
{
	class sprite
	{
	public:
		explicit sprite(const asset<chirp::texture> &texture);

		explicit sprite(const asset<chirp::image> &image);

		void draw() const;

		[[nodiscard]]
		auto get_size() const -> vector2i;

		[[nodiscard]]
		auto get_position() const -> const vector2f &;

		void set_position(const vector2f &value);

	private:
		asset<chirp::texture> texture;
		vector2f position;
	};
}
