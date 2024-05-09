#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "msgpack.hpp"
#include "argparse/argparse.hpp"
#include "toml.hpp"

auto read_file(const std::filesystem::path &path) -> std::vector<unsigned char>
{
	std::ifstream file(path, std::ios::binary);
	return {
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	};
}

auto pack(const std::string &input, const bool simulate) -> int
{
	const auto base_path = std::filesystem::path(input).parent_path();
	const auto toml = toml::parse(input);
	const auto assets = toml::find<toml::table>(toml, "assets");

	std::map<std::string, std::map<std::string, std::vector<unsigned char>>> data;

	for (const auto &[asset_key, asset_value]: assets)
	{
		data[asset_key] = std::map<std::string, std::vector<unsigned char>>();

		const auto files = toml::get<toml::table>(asset_value);
		for (const auto &[file_key, file_value]: files)
		{
			const auto path = toml::get<std::string>(file_value);
			std::cout << "< " << path << " < " << asset_key << '/' << file_key << '\n';
			data[asset_key][file_key] = read_file(base_path / path);
		}
	}

	std::stringstream stream;
	msgpack::pack(stream, data);

	const std::filesystem::path output(base_path / "assets.nest");
	std::cout << "> " << output << '\n';

	if (!simulate)
	{
		std::ofstream file(output);
		file << stream.str();
		file.close();
	}

	return 0;
}

auto unpack_file(const std::string &input) -> std::map<std::string, std::map<std::string, std::vector<unsigned char>>>
{
	const std::filesystem::path path(input);
	if (!exists(path) || !is_regular_file(path))
	{
		std::cerr << "error: file does not exist!\n";
		std::exit(1);
	}

	const auto data = read_file(path);
	const auto handle = msgpack::unpack(reinterpret_cast<const char *>(data.data()), data.size());
	const auto obj = handle.get();

	std::map<std::string, std::map<std::string, std::vector<unsigned char>>> map;

	try
	{
		obj.convert(map);
	}
	catch (std::exception &exc)
	{
		std::cerr << "error: file doesn't appear to be a valid nest file (" << exc.what() << ")\n";
		std::exit(1);
	}

	return map;
}

auto unpack(const std::string &input, const bool simulate) -> int
{
	const auto file_map = unpack_file(input);

	const std::filesystem::path input_path(input);
	std::cout << "< " << input_path.string() << '\n';

	const auto base_path = input_path.parent_path() / input_path.stem();
	create_directories(base_path);

	for (const auto &[folder, content]: file_map)
	{
		for (const auto &[filename, data]: content)
		{
			const auto out_path = std::filesystem::path(folder) / filename;
			std::cout << "> " << (input_path.stem() / out_path).string() << '\n';

			if (simulate)
			{
				continue;
			}

			create_directories(base_path / folder);
			std::ofstream file(base_path / out_path, std::ios::binary);
			file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
			file.close();
		}
	}

	return 0;
}

auto main(const int argc, const char **argv) -> int
{
	argparse::ArgumentParser parser(APP_NAME, APP_VERSION);

	auto &mode_group = parser.add_mutually_exclusive_group();

	mode_group.add_argument("-p", "--pack")
		.help("pack assets")
		.default_value(std::string("project.toml"));

	mode_group.add_argument("-u", "--unpack")
		.help("unpack assets")
		.default_value(std::string("assets.nest"));

	parser.add_argument("-s", "--simulate")
		.help("don't create or modify any files")
		.flag();

	try
	{
		parser.parse_args(argc, argv);
	}
	catch (const std::exception &exc)
	{
		std::cerr << exc.what() << '\n';
		std::cerr << parser;
		return 1;
	}

	const auto simulate = parser["simulate"] == true;

	if (parser.is_used("pack"))
	{
		const auto project_file = parser.get<std::string>("pack");
		return pack(project_file, simulate);
	}

	if (parser.is_used("unpack"))
	{
		const auto asset_file = parser.get<std::string>("unpack");
		return unpack(asset_file, simulate);
	}

	return 1;
}
