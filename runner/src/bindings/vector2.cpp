#include "bindings.hpp"

#include <chirp/format.hpp>
#include <chirp/vector2.hpp>

#include <pybind11/operators.h>

template<typename T>
auto bind_vector2(const pybind11::module &module, const char *suffix) -> pybind11::class_<chirp::vector2<T>>
{
	const auto name = chirp::format("Vector2{}", suffix);

	return pybind11::class_<chirp::vector2<T>>(module, name.c_str())
		.def(pybind11::init<T, T>())
		.def(pybind11::self + T())
		.def(pybind11::self + pybind11::self)
		.def(pybind11::self - T())
		.def(pybind11::self - pybind11::self)
		.def(pybind11::self * T())
		// .def(pybind11::self * pybind11::self) // TODO: Not implemented
		.def(pybind11::self / T())
		// .def(pybind11::self / pybind11::self) // TODO: Not implemented
		.def_property_readonly("x", &chirp::vector2<T>::x)
		.def_property_readonly("y", &chirp::vector2<T>::y)
		.def("__str__", [](const chirp::vector2<T> &vec)
		{
			return chirp::fmt::to_string(vec);
		});
}

void bind_vector2(const pybind11::module &module)
{
	bind_vector2<int>(module, "i");
	bind_vector2<float>(module, "f");
}
