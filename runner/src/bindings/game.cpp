#include "bindings.hpp"

#include <chirp/game.hpp>

void bind_game(const pybind11::module_ &module)
{
	pybind11::class_<chirp::game>(module, "Game")
		.def(pybind11::init<>())
		.def("run", &chirp::game::run)
		.def("exit", &chirp::game::exit)
		.def_property_readonly("window", &chirp::game::window)
		.def_property_readonly("scenes", &chirp::game::scenes)
		.def_property("target_fps", &chirp::game::get_target_fps, &chirp::game::set_target_fps)
		.def_property("clear_color", &chirp::game::get_clear_color, &chirp::game::set_clear_color);
}
