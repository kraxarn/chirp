#include "bindings.hpp"

#include <chirp/sound.hpp>

void bind_sound(const pybind11::module &module)
{
	pybind11::pyclass<chirp::sound>(module, "Sound")
		.def("play", &chirp::sound::play)
		.def_property_readonly("playing", &chirp::sound::is_playing);
}
