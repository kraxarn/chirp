#include "bindings.hpp"

#include <chirp/scene.hpp>

void bind_scene(const pybind11::module &module)
{
	pybind11::class_<chirp::scene>(module, "Scene")
		.def(pybind11::init<>())
		.def("load", &chirp::scene::load)
		.def("update", &chirp::scene::update)
		.def("draw", &chirp::scene::draw)
		.def_property_readonly("scenes", &chirp::scene::scenes)
		.def_property_readonly("window", &chirp::scene::window)
		.def_property_readonly("assets", &chirp::scene::assets);
}
