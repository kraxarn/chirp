#include "chirp/entitycontainer.hpp"
#include "chirp/camera.hpp"

void chirp::entity_container::append_entity(const std::string &name, const chirp::handle<chirp::entity> &handle)
{
	if (contains(name))
	{
		chirp::log::fatal("entity already exists: {}", name);
		return;
	}

	entitites[name] = handle;
	entity_order.push_back(handle);
}

auto chirp::entity_container::remove(const std::string &name) -> bool
{
	if (!entitites.contains(name))
	{
		return false;
	}

	const auto entity = entitites.at(name);
	if (entitites.erase(name) == 0)
	{
		return false;
	}

	const auto position = std::ranges::remove(entity_order, entity).begin();
	const auto old_size = entity_order.size();
	entity_order.erase(position, entity_order.end());
	return entity_order.size() < old_size;
}

auto chirp::entity_container::entities() const -> const std::vector<chirp::handle<chirp::entity>> &
{
	return entity_order;
}

auto chirp::entity_container::entity_keys() const -> std::vector<std::string>
{
	std::vector<std::string> names;
	names.reserve(entitites.size());

	for (const auto &[name, entity]: entitites)
	{
		names.push_back(name);

		const auto camera = std::dynamic_pointer_cast<chirp::camera>(entity);
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
	return entitites.contains(name);
}
