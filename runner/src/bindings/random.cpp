#include "bindings.hpp"

#include <chirp/random.hpp>

void bind_random(pkpy::VM *py_vm)
{
	auto *mod = py_vm->new_module("random", "chirp");

	py_vm->bind(mod, "get(min, max)", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto min = pkpy::py_cast<int>(pkpy_vm, args[0]);
		const auto max = pkpy::py_cast<int>(pkpy_vm, args[1]);
		return py_var(pkpy_vm, chirp::random::get(min, max));
	});
}
