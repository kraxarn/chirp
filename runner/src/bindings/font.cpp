#include "bindings.hpp"

#include <chirp/font.hpp>

void bind_font(const pybind11::module &module)
{
	pybind11::pyclass<chirp::font>(module, "Font")
		.def_property_readonly("font_size", &chirp::font::font_size);
}
