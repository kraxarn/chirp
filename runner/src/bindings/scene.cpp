#include "bindings.hpp"

#include <chirp/scene.hpp>
#include <chirp/scenemanager.hpp>

void bind_scene(const pybind11::module &module)
{
	pybind11::pyclass<chirp::scene>(module, "Scene")
		.def(pybind11::init<>())
		.def("load", &chirp::scene::load)
		.def("update", &chirp::scene::update)
		.def("draw", &chirp::scene::draw)
		.def_property_readonly("scenes", &chirp::scene::scenes)
		.def_property_readonly("window", &chirp::scene::window)
		.def_property_readonly("assets", &chirp::scene::assets)
		// entity_container
		.def("append", &chirp::scene::append_unmanaged)
		.def("queue_append", &chirp::scene::queue_append<chirp::entity>)
		.def("queue_remove", &chirp::scene::queue_remove)
		.def("find", &chirp::scene::find<chirp::entity>)
		.def_property_readonly("entities", &chirp::scene::entities)
		.def_property_readonly("entity_keys", &chirp::scene::entity_keys)
		.def("contains", &chirp::scene::contains);
}
