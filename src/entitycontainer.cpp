#include "chirp/entitycontainer.hpp"

void chirp::entity_container::insert_asset(const std::string &name, const chirp::asset<chirp::entity> &asset)
{
	entitites[name] = asset;
	entity_order.push_back(asset);
}

auto chirp::entity_container::erase(const std::string &name) -> bool
{
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

auto chirp::entity_container::at(const std::string &name) -> const chirp::asset<chirp::entity> &
{
	return entitites.at(name);
}

auto chirp::entity_container::items() const -> const std::vector<chirp::asset<chirp::entity>> &
{
	return entity_order;
}
