#include "chirp/assets.hpp"

#include "chirp/format.hpp"

#include "msgpack.hpp"
#include "raylib.h"

chirp::assets::assets()
{
	int data_size;
	const auto *file_data = reinterpret_cast<char *>(LoadFileData("assets.nest", &data_size));
	if (data_size <= 0)
	{
		return;
	}

	const auto handle = msgpack::unpack(file_data, static_cast<size_t>(data_size));
	const auto object = handle.get();
	object.convert(data);
}

auto chirp::assets::load(const std::string &type, const std::string &name) const -> const std::vector<unsigned char> &
{
	if (!data.contains(type))
	{
		const auto message = chirp::format("invalid type: {}", type);
		throw std::runtime_error(message);
	}

	if (!data.at(type).contains(name))
	{
		const auto message = chirp::format("invalid asset: {}/{}", type, name);
		throw std::runtime_error(message);
	}

	return data.at(type).at(name);
}

auto chirp::assets::music(const std::string &path) const -> asset<chirp::music>
{
	return std::make_shared<chirp::music>(load("music", path), path);
}

auto chirp::assets::sound(const std::string &path) const -> asset<chirp::sound>
{
	return std::make_shared<chirp::sound>(load("sounds", path));
}

auto chirp::assets::font(const std::string &path, const int font_size) const -> asset<chirp::font>
{
	return std::make_shared<chirp::font>(load("fonts", path), font_size);
}

auto chirp::assets::image(const std::string &path) const -> asset<chirp::image>
{
	return std::make_shared<chirp::image>(load("images", path));
}

auto chirp::assets::tileset(const std::string &path) const -> asset<chirp::tileset>
{
	return std::make_shared<chirp::tileset>(load("tilesets", path));
}

auto chirp::assets::level(const std::string &path) const -> chirp::asset<chirp::level>
{
	return std::make_shared<chirp::level>(load("levels", path));
}
