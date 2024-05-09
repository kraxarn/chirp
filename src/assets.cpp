#include "chirp/assets.hpp"

#include "format.hpp"

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
