#pragma once

#include <pybind11/pybind11.h>

// Just to avoid _ suffix
namespace pybind11
{
	using module = module_;
}

void bind_game(const pybind11::module &module);
void bind_os(pybind11::module &module);
void bind_scene(const pybind11::module &module);
