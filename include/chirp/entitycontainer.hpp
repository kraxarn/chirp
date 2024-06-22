#pragma once

#include <chirp/asset.hpp>
#include <chirp/entity.hpp>
#include <chirp/log.hpp>

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

		template<typename T>
		void insert(const std::string &name, const chirp::asset<T> &asset)
		{
			insert_asset(name, asset);
		}

		auto erase(const std::string &name) -> bool;

		template<typename T>
		[[nodiscard]]
		auto at(const std::string &name) -> chirp::asset<T>
		{
			if (!contains(name))
			{
				log::fatal("No such entity: {}", name);
			}

			auto entity = std::dynamic_pointer_cast<T>(entitites.at(name));
			if (!entity)
			{
				log::fatal("Wrong entity type: {}", name);
			}

			return entity;
		}

		[[nodiscard]]
		auto items() const -> const std::vector<chirp::asset<chirp::entity>> &;

		[[nodiscard]]
		auto names() const -> std::vector<std::string>;

		[[nodiscard]]
		auto contains(const std::string &name) const -> bool;

	private:
		std::unordered_map<std::string, chirp::asset<chirp::entity>> entitites;
		std::vector<chirp::asset<chirp::entity>> entity_order;

		void insert_asset(const std::string &name, const chirp::asset<chirp::entity> &asset);
	};
}
