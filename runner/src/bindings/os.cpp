#include "bindings.hpp"

#include <chirp/os.hpp>

void bind_os(pkpy::VM *py_vm)
{
	auto *mod = py_vm->new_module("os", "chirp");

	py_vm->bind(mod, "is_debug()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView /*args*/) -> pkpy::PyVar
	{
		return pkpy::py_var(pkpy_vm, chirp::os::is_debug());
	});
}
