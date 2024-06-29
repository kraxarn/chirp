#include "bindings.hpp"

#include <chirp/music.hpp>

void bind_music(const pybind11::module &module)
{
	pybind11::pyclass<chirp::music>(module, "Music")
		.def_property_readonly("playing", &chirp::music::is_playing)
		.def_property_readonly("name", &chirp::music::name);
}
