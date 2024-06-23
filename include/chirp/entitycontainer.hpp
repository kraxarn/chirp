#pragma once

#include <chirp/handle.hpp>
#include <chirp/log.hpp>
#include <chirp/internal/entityaction.hpp>

#include <queue>
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

		template<typename T>
		chirp::handle<T> queue_append(const std::string &name, T *entity)
		{
			auto handle = std::shared_ptr<T>(entity);

			entity_queue.push(internal::append_action{
				.name = name,
				.entity = handle,
			});

			return handle;
		}

		void queue_remove(const std::string &name);

		template<typename T>
		[[nodiscard]]
		auto find(const std::string &name) const -> handle<T>
		{
			if (!contains(name))
			{
				log::warn("No such entity: {}", name);
				return {};
			}

			auto entity = std::dynamic_pointer_cast<T>(entitites.at(name));
			if (!entity)
			{
				log::warn("Wrong entity type: {}", name);
				return {};
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

		void clear_queue();

	private:
		std::unordered_map<std::string, handle<entity>> entitites;
		std::vector<handle<entity>> entity_order;
		std::queue<internal::entity_action> entity_queue;

		void append_entity(const std::string &name, const handle<entity> &handle);
		auto remove_entity(const std::string &name) -> bool;
	};
}
