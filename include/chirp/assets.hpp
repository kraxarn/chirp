#pragma once

#include <chirp/asset.hpp>
#include <chirp/font.hpp>
#include <chirp/image.hpp>
#include <chirp/level.hpp>
#include <chirp/music.hpp>
#include <chirp/sound.hpp>
#include <chirp/tileset.hpp>

#include <map>

namespace chirp
{
	class assets
	{
	public:
		assets();

		[[nodiscard]]
		auto music(const std::string &path) const -> asset<chirp::music>;

		[[nodiscard]]
		auto sound(const std::string &path) const -> asset<chirp::sound>;

		[[nodiscard]]
		auto font(const std::string &path, int font_size) const -> asset<chirp::font>;

		[[nodiscard]]
		auto image(const std::string &path) const -> asset<chirp::image>;

		[[nodiscard]]
		auto tileset(const std::string &path) const -> asset<chirp::tileset>;

		[[nodiscard]]
		auto level(const std::string &path) const -> asset<chirp::level>;

	private:
		std::map<std::string, std::map<std::string, std::vector<unsigned char>>> data;

		[[nodiscard]]
		auto load(const std::string &type, const std::string &name) const -> const std::vector<unsigned char> &;
	};
}
