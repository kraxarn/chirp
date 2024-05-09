#pragma once

#include <map>

namespace chirp
{
	class assets
	{
	public:
		assets();

		auto load(const std::string &type, const std::string &name) const -> const std::vector<unsigned char> &;

	private:
		std::map<std::string, std::map<std::string, std::vector<unsigned char>>> data;
	};
}
