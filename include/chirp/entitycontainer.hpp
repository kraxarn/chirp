#pragma once

#include <chirp/handle.hpp>
#include <chirp/log.hpp>

#include <unordered_map>
#include <vector>

namespace chirp
{
	class entity;

	class entity_container
	{
	public:
		template<typename T>
		chirp::handle<T> append(const std::string &name, T *entity)
		{
			auto handle = std::shared_ptr<T>(entity);
			append_entity(name, handle);
			return handle;
		}

		auto remove(const std::string &name) -> bool;

		template<typename T>
		[[nodiscard]]
		auto find(const std::string &name) const -> handle<T>
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
		auto entities() const -> const std::vector<handle<entity>> &;

		[[nodiscard]]
		auto entity_keys() const -> std::vector<std::string>;

		[[nodiscard]]
		auto contains(const std::string &name) const -> bool;

	protected:
		entity_container() = default;

	private:
		std::unordered_map<std::string, handle<entity>> entitites;
		std::vector<handle<entity>> entity_order;

		void append_entity(const std::string &name, const handle<entity> &handle);
	};
}
