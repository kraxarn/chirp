#pragma once

#include <chirp/entity.hpp>
#include <chirp/handle.hpp>
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
		chirp::handle<T> insert(const std::string &name, T *entity)
		{
			auto handle = std::shared_ptr<T>(entity);
			insert_entity(name, handle);
			return handle;
		}

		auto erase(const std::string &name) -> bool;

		template<typename T>
		[[nodiscard]]
		auto find(const std::string &name) -> handle<T>
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
		auto items() const -> const std::vector<handle<entity>> &;

		[[nodiscard]]
		auto names() const -> std::vector<std::string>;

		[[nodiscard]]
		auto contains(const std::string &name) const -> bool;

	private:
		std::unordered_map<std::string, handle<entity>> entitites;
		std::vector<handle<entity>> entity_order;

		void insert_entity(const std::string &name, const handle<entity> &handle);
	};
}
