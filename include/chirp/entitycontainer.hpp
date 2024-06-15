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

		template<typename T>
		void insert(const std::string &name, const T &entity)
		{
			const auto asset = std::make_shared<T>(entity);
			insert_asset(name, asset);
		}

		template<>
		void insert(const std::string &name, const chirp::asset<chirp::entity> &entity)
		{
			insert_asset(name, entity);
		}

		auto erase(const std::string &name) -> bool;

		template<typename T>
		[[nodiscard]]
		auto at(const std::string &name) -> chirp::asset<T>
		{
			return std::dynamic_pointer_cast<T>(entitites.at(name));
		}

		[[nodiscard]]
		auto items() const -> const std::vector<chirp::asset<chirp::entity>> &;

	private:
		std::unordered_map<std::string, chirp::asset<chirp::entity>> entitites;
		std::vector<chirp::asset<chirp::entity>> entity_order;

		void insert_asset(const std::string &name, const chirp::asset<chirp::entity> &asset);
	};
}
