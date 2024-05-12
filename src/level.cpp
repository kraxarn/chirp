#include "chirp/level.hpp"

#include <sstream>

chirp::level::level(const std::vector<unsigned char> &data)
	: map_data({})
{
	parse(data);
}

auto chirp::level::name() const -> const std::string &
{
	return name_data;
}

auto chirp::level::tileset() const -> const std::string &
{
	return tileset_data;
}

auto chirp::level::music() const -> const std::string &
{
	return music_data;
}

auto chirp::level::map() const -> const chirp::map &
{
	return map_data;
}

auto chirp::level::type() const -> const std::string &
{
	return level_type;
}

void chirp::level::set_tile(const int x_pos, const int y_pos, const char value)
{
	map_data.at(x_pos).at(y_pos) = value;
}

void chirp::level::iterate(const std::function<bool(const map_tile &)> &iter) const
{
	for (size_t pos_x = 0; pos_x < map().size(); pos_x++)
	{
		for (size_t pos_y = 0; pos_y < map().at(pos_x).size(); pos_y++)
		{
			if (!iter({pos_x, pos_y, map().at(pos_x).at(pos_y)}))
			{
				return;
			}
		}
	}
}

void chirp::level::parse(const std::vector<unsigned char> &level_data)
{
	std::string data(level_data.cbegin(), level_data.cend());
	std::istringstream stream(data);
	std::string line;

	while (std::getline(stream, line))
	{
		const auto split = line.find('=');
		if (split != std::string::npos)
		{
			const auto key = line.substr(0, split);
			const auto value = line.substr(split + 1);

			if (key == "name")
			{
				name_data = value;
			}
			else if (key == "tileset")
			{
				tileset_data = value;
			}
			else if (key == "music")
			{
				music_data = value;
			}
			else if (key == "type")
			{
				level_type = value;
			}
			continue;
		}

		if (line == "[map]")
		{
			size_t y_index = 0;

			while (std::getline(stream, line))
			{
				size_t x_index = 0;
				size_t start = 0;
				size_t end = line.find(' ');

				while (end != std::string::npos)
				{
					map_data.at(y_index).at(x_index) = parse_tile(line, start, end);
					start = end + 1;
					end = line.find(' ', start);
					x_index++;
				}

				map_data.at(y_index).at(x_index) = parse_tile(line, start, end);
				y_index++;
			}
		}
	}
}

auto chirp::level::parse_tile(const std::string &line, const size_t start, const size_t end) -> char
{
	const auto str_val = line.substr(start, end - start);
	const auto int_val = std::stoi(str_val);
	return static_cast<char>(int_val);
}
