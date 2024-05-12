#pragma once

#include <array>

namespace chirp
{
	/**
	 * Maximum number of tiles horizontally
	 */
	static constexpr auto level_tiles_width = 75;

	/**
	 * Maximum number of tiles vertically
	 */
	static constexpr auto level_tiles_height = 25;

	/**
	 * Size of each tile in tileset
	 */
	static constexpr auto tileset_size = 18.F;

	/**
	 * Level map data
	 */
	using map = std::array<std::array<char, level_tiles_height>, level_tiles_width>;

	/**
	 * A tile in the level map
	 */
	using map_tile = struct
	{
		size_t x;
		size_t y;
		char value;
	};
}