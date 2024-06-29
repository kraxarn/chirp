#include "bindings.hpp"

#include <chirp/tileset.hpp>

void bind_tileset(const pybind11::module &module)
{
	pybind11::pyclass<chirp::tileset>(module, "Tileset");
}
