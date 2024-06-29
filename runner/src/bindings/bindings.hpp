#pragma once

#include <pybind11/pybind11.h>

void bind_game(const pybind11::module_ &module);
void bind_os(pybind11::module_ &module);
void bind_scene(const pybind11::module_ &module);
