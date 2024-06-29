#include "bindings.hpp"

#include <chirp/level.hpp>

void bind_level(const pybind11::module &module)
{
	pybind11::pyclass<chirp::level>(module, "Level")
		.def("set_tile", &chirp::level::set_tile)
		.def_property_readonly("name", &chirp::level::name)
		.def_property_readonly("tileset", &chirp::level::tileset)
		.def_property_readonly("music", &chirp::level::music)
		.def_property_readonly("map", &chirp::level::map)
		.def_property_readonly("type", &chirp::level::type);
}
