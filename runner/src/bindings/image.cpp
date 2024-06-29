#include "bindings.hpp"

#include <chirp/image.hpp>

void bind_image(const pybind11::module &module)
{
	pybind11::pyclass<chirp::image>(module, "Image")
		.def("flip_horizontal", &chirp::image::flip_horizontal);
}
