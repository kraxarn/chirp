#include "bindings.hpp"

#include <chirp/os.hpp>

void bind_os(pybind11::module &module)
{
	auto sub_module = module.def_submodule("os");

	sub_module.def("is_debug", &chirp::os::is_debug);
}
