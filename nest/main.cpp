#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "msgpack.hpp"

void print_help(const char *path)
{
	std::cout << "usage: " << path << " [options] assets\n\n"
		<< "options:\n"
		<< "  -p, --pack    pack assets\n"
		<< "  -u, --unpack  unpack assets\n"
		<< "  -o, --output  output content to stdout\n";
}

auto read_file(const std::filesystem::path &path) -> std::vector<char>
{
	std::ifstream file(path, std::ios::binary);

	file.seekg(0, std::ios::end);
	const auto filesize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(filesize);
	file.read(buffer.data(), filesize);
	return buffer;
}

auto pack(const char *input) -> int
{
	const std::filesystem::path path(input);
	if (!exists(path) || !is_directory(path))
	{
		std::cerr << "error: directory does not exist!\n";
		return 1;
	}

	std::map<std::string, std::map<std::string, std::vector<char>>> data;

	for (const auto &entry: std::filesystem::recursive_directory_iterator(path))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		const auto &entry_path = entry.path();

		const auto &folder = entry_path.parent_path().stem().string();
		const auto &filename = entry_path.filename().string();

		if (!data.contains(folder))
		{
			data[folder] = std::map<std::string, std::vector<char>>();
		}

		data[folder][filename] = read_file(entry_path);
		std::cout << "< " << folder << '/' << filename << '\n';
	}

	std::stringstream stream;
	msgpack::pack(stream, data);

	std::filesystem::path output(path.parent_path() / path.stem());
	output.replace_extension("nest");
	std::cout << "> " << output << '\n';

	std::ofstream file(output);
	file << stream.str();
	file.close();

	return 0;
}

auto unpack_file(const char *input) -> std::map<std::string, std::map<std::string, std::vector<char>>>
{
	const std::filesystem::path path(input);
	if (!exists(path) || !is_regular_file(path))
	{
		std::cerr << "error: file does not exist!\n";
		std::exit(1);
	}

	const auto data = read_file(path);
	const auto handle = msgpack::unpack(data.data(), data.size());
	const auto obj = handle.get();

	std::map<std::string, std::map<std::string, std::vector<char>>> map;
	obj.convert(map);
	return map;
}

auto unpack(const char *input) -> int
{
	const std::filesystem::path input_path(input);
	std::cout << "< " << input_path.string() << '\n';

	const auto base_path = input_path.parent_path() / input_path.stem();
	create_directories(base_path);

	for (const auto &[folder, content]: unpack_file(input))
	{
		for (const auto &[filename, data]: content)
		{
			const auto out_path = std::filesystem::path(folder) / filename;
			std::cout << "> " << (input_path.stem() / out_path).string() << '\n';

			create_directories(base_path / folder);
			std::ofstream file(base_path / out_path, std::ios::binary);
			file.write(data.data(), static_cast<std::streamsize>(data.size()));
			file.close();
		}
	}

	return 0;
}

auto output(const char *input) -> int
{
	for (const auto &[folder, content]: unpack_file(input))
	{
		std::cout << folder << '\n';

		for (const auto &[filename, data]: content)
		{
			const auto filesize = data.size();
			std::cout << "  " << filename << " (" << filesize << " bytes)\n";
		}
	}

	return 0;
}

auto main(const int argc, const char **argv) -> int
{
	if (argc <= 2)
	{
		print_help(argv[0]);
		return 1;
	}

	if (std::strcmp(argv[1], "-p") == 0 || std::strcmp(argv[1], "--pack") == 0)
	{
		return pack(argv[2]);
	}

	if (std::strcmp(argv[1], "-u") == 0 || std::strcmp(argv[1], "--unpack") == 0)
	{
		return unpack(argv[2]);
	}

	if (std::strcmp(argv[1], "-o") == 0 || std::strcmp(argv[1], "--output") == 0)
	{
		return output(argv[2]);
	}

	print_help(argv[0]);
	return 1;
}
