#include "bindings.hpp"

#include <chirp/window.hpp>

void bind_window(const pybind11::module &module)
{
	pybind11::pyclass<chirp::window>(module, "Window")
		.def_property("title", &chirp::window::get_title, &chirp::window::set_title)
		.def_property("size", &chirp::window::get_size, &chirp::window::set_size);
}
