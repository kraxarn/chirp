#include "bindings.hpp"

#include <chirp/color.hpp>
#include <chirp/format.hpp>

void bind_color(const pybind11::module_ &module)
{
	pybind11::class_<chirp::color>(module, "Color")
		.def(pybind11::init<>())
		.def(pybind11::init<unsigned char, unsigned char, unsigned char>())
		.def(pybind11::init<unsigned char, unsigned char, unsigned char, unsigned char>())
		.def_property_readonly("red", &chirp::color::red)
		.def_property_readonly("green", &chirp::color::green)
		.def_property_readonly("blue", &chirp::color::blue)
		.def_property_readonly("alpha", &chirp::color::alpha)
		.def("__str__", [](const chirp::color &color)
		{
			return chirp::fmt::to_string(color);
		});
}
