#include "chirp/entitycontainer.hpp"
#include "chirp/camera.hpp"

void chirp::entity_container::append_entity(const std::string &name, chirp::handle<chirp::entity> &handle)
{
	if (contains(name))
	{
		chirp::log::fatal("Entity already exists: {}", name);
		return;
	}

	if (const auto index = name.find('/'); index != std::string::npos)
	{
		const auto parent_name = name.substr(0, index);
		if (const auto &parent = find<chirp::entity_container>(parent_name))
		{
			parent->append_entity(name.substr(index + 1), handle);
			return;
		}
	}

	entity_map[name] = handle;
	entity_order.push_back(handle);
}

void chirp::entity_container::queue_remove(const std::string &name)
{
	entity_queue.emplace(internal::remove_action{
		.name = name,
	});
}

auto chirp::entity_container::remove_entity(const std::string &name) -> bool
{
	if (const auto index = name.find('/'); index != std::string::npos)
	{
		const auto parent_name = name.substr(0, index);
		if (const auto &parent = find<chirp::entity_container>(parent_name))
		{
			return parent->remove_entity(name.substr(index + 1));
		}
	}

	if (!entity_map.contains(name))
	{
		return false;
	}

	const auto entity = entity_map.at(name);
	if (entity_map.erase(name) == 0)
	{
		return false;
	}

	const auto position = std::ranges::remove(entity_order, entity).begin();
	const auto old_size = entity_order.size();
	entity_order.erase(position, entity_order.end());
	return entity_order.size() < old_size;
}

void chirp::entity_container::clear_queue()
{
	while (!entity_queue.empty())
	{
		auto &action = entity_queue.front();

		if (const auto *remove_action = std::get_if<internal::remove_action>(&action))
		{
			remove_entity(remove_action->name);
		}
		else if (auto *append_action = std::get_if<internal::append_action>(&action))
		{
			append_entity(append_action->name, append_action->entity);
		}

		entity_queue.pop();
	}
}

auto chirp::entity_container::entities() const -> const std::vector<chirp::handle<chirp::entity>> &
{
	return entity_order;
}

auto chirp::entity_container::entity_keys() const -> std::vector<std::string>
{
	std::vector<std::string> names;
	names.reserve(entity_map.size());

	for (const auto &[name, entity]: entity_map)
	{
		names.push_back(name);

#ifdef NO_MANAGE_MEMORY
		const auto *camera = dynamic_cast<chirp::camera *>(entity);
#else
		const auto camera = std::dynamic_pointer_cast<chirp::camera>(entity);
#endif

		if (!camera)
		{
			continue;
		}

		for (const auto &camera_name: camera->entity_keys())
		{
			names.push_back(chirp::format("{}/{}", name, camera_name));
		}
	}

	return names;
}

auto chirp::entity_container::contains(const std::string &name) const -> bool
{
	return entity_map.contains(name);
}
