#pragma once

#include <chirp/asset.hpp>
#include <chirp/image.hpp>
#include <chirp/vector2.hpp>

struct Texture;

namespace chirp
{
	class texture
	{
	public:
		explicit texture(const asset<chirp::image> &image);
		~texture();

		/**
		 * Draw entire texture at position
		 * @param position Draw position
		 */
		void draw(const vector2f &position) const;

		/**
		 * Draw part of texture
		 * @param position Draw position
		 * @param size Texture width/height
		 * @param offset X offset in position
		 */
		void draw(const vector2f &position, const vector2i &size, int offset) const;

		/**
		 * Draw part of texture with rotation and scale
		 * @param position Draw position
		 * @param size Texture width/height
		 * @param offset X offset in position
		 * @param rotation Texture rotation in degrees
		 */
		void draw(const vector2f &position, const vector2i &size, int offset,
			float rotation, float scale) const;

		/**
		 * Size of entire texture
		 */
		[[nodiscard]]
		auto size() const -> vector2i;

		/**
		 * Flip the image and reload the texture
		 */
		void flip_horizontal() const;

	private:
		asset<chirp::image> image;
		Texture *r_texture;

		void reload() const;
	};
}
