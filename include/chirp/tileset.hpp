#pragma once

#include <chirp/image.hpp>
#include <chirp/texture.hpp>
#include <chirp/vector2.hpp>

#include <vector>

namespace chirp
{
	class tileset
	{
	public:
		explicit tileset(const std::vector<unsigned char> &data);

		[[nodiscard]]
		auto at(int index) const -> chirp::image;

		void draw(const vector2f &position, int index, float rotation, float scale) const;

		void flip_horizontal() const;

		[[nodiscard]]
		auto get_size() const -> int;

	private:
		/**
		 * Total size for each tile
		 */
		vector2i size;

		asset<chirp::image> image;
		chirp::texture texture;
	};
}
