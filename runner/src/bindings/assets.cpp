#include "bindings.hpp"

#include <chirp/assets.hpp>

void bind_assets(const pybind11::module &module)
{
	pybind11::pyclass<chirp::assets>(module, "Assets")
		.def("music", &chirp::assets::music)
		.def("sound", &chirp::assets::sound)
		.def("font", &chirp::assets::font)
		.def("image", &chirp::assets::image)
		.def("level", &chirp::assets::level);
}
