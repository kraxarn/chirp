#pragma once

#include <chirp/map.hpp>
#include <chirp/map_iterator.hpp>

#include <functional>
#include <vector>

namespace chirp
{
	class level
	{
	public:
		explicit level(const std::vector<unsigned char> &data);

		[[nodiscard]]
		auto name() const -> const std::string &;

		[[nodiscard]]
		auto tileset() const -> const std::string &;

		[[nodiscard]]
		auto music() const -> const std::string &;

		[[nodiscard]]
		auto map() const -> const map &;

		void set_tile(int x_pos, int y_pos, char value);

		[[nodiscard]]
		auto type() const -> const std::string &;

		/**
		 * Iterate through tiles in level
		 */
		[[nodiscard]]
		auto tiles() const -> map_iterator;

	private:
		std::string name_data;
		std::string tileset_data;
		std::string music_data;
		std::string level_type;
		chirp::map map_data;

		void parse(const std::vector<unsigned char> &level_data);
		static auto parse_tile(const std::string &line, size_t start, size_t end) -> char;
	};
}
