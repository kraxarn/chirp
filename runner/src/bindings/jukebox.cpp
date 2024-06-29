#include "bindings.hpp"

#include <chirp/jukebox.hpp>

void bind_jukebox(const pybind11::module &module)
{
	pybind11::pyclass<chirp::jukebox>(module, "Jukebox")
		.def("insert", &chirp::jukebox::insert)
		.def("play", &chirp::jukebox::play)
		.def("stop", &chirp::jukebox::stop)
		.def_property_readonly("empty", &chirp::jukebox::empty)
		.def_property("volume", &chirp::jukebox::get_volume, &chirp::jukebox::set_volume)
		.def_property_readonly("name", &chirp::jukebox::name);
}
