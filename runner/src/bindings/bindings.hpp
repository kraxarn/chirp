#pragma once

#include <pybind11/pybind11.h>

// Just to avoid _ suffix
namespace pybind11
{
	using module = module_;

	template<typename T>
	using pyclass = class_<T>;
}

// chirp
void bind_color(const pybind11::module &module);
void bind_game(const pybind11::module &module);
void bind_scene(const pybind11::module &module);
void bind_scene_manager(const pybind11::module &module);
void bind_vector2(const pybind11::module &module);
void bind_window(const pybind11::module &module);

// chirp.os
void bind_os(pybind11::module &module);
