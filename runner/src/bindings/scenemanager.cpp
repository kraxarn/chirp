#include "bindings.hpp"

#include <chirp/scenemanager.hpp>

void bind_scene_manager(const pybind11::module &module)
{
	pybind11::class_<chirp::scene_manager>(module, "SceneManager")
		.def(pybind11::init<>())
		.def("push", &chirp::scene_manager::push<chirp::scene>)
		.def("queue_push", &chirp::scene_manager::queue_push)
		.def("queue_pop", &chirp::scene_manager::queue_pop)
		.def("empty", &chirp::scene_manager::empty)
		.def("peek", &chirp::scene_manager::peek);
}
