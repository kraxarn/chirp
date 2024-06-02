#pragma once

#include <chirp/map.hpp>

namespace chirp
{
	class map_iterator
	{
	public:
		explicit map_iterator(const map &map)
			: map(map)
		{
		}

		[[nodiscard]]
		auto begin() const -> map_iterator
		{
			return map_iterator(map);
		}

		[[nodiscard]]
		auto end() const -> map_iterator
		{
			auto iter = map_iterator(map);
			iter.cur_x = map.size();
			return iter;
		}

		auto operator++() -> map_iterator &
		{
			cur_y++;

			if (cur_y >= map.at(cur_x).size())
			{
				cur_y = 0;
				cur_x++;
			}

			return *this;
		}

		auto operator==(const map_iterator &iter) const -> bool
		{
			return map == iter.map
				&& cur_x == iter.cur_x
				&& cur_y == iter.cur_y;
		}

		auto operator*() const -> map_tile
		{
			return map_tile
			{
				.x = cur_x,
				.y = cur_y,
				.value = map.at(cur_x).at(cur_y),
			};
		}

	private:
		const chirp::map &map;
		unsigned char cur_x = 0;
		unsigned char cur_y = 0;
	};
}
