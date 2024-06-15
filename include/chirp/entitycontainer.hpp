#pragma once

#include <chirp/asset.hpp>
#include <chirp/entity.hpp>

#include <unordered_map>
#include <vector>

namespace chirp
{
	class entity_container
	{
	public:
		entity_container() = default;

		void insert(const std::string &name, const chirp::asset<chirp::entity> &entity);

		auto erase(const std::string &name) -> bool;

		[[nodiscard]]
		auto at(const std::string &name) -> const chirp::asset<chirp::entity> &;

		[[nodiscard]]
		auto items() const -> const std::vector<chirp::asset<chirp::entity>> &;

	private:
		std::unordered_map<std::string, chirp::asset<chirp::entity>> entitites;

		std::vector<chirp::asset<chirp::entity>> entity_order;
	};
}
