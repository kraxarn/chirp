#include "bindings.hpp"

#include <chirp/scene.hpp>

void bind_scene(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<chirp::scene>(mod, "Scene",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "load(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					return py_vm->None;
				});

			pkpy_vm->bind(type, "update(self, delta)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView /*args*/) -> pkpy::PyVar
				{
					return py_vm->None;
				});

			pkpy_vm->bind(type, "draw(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView /*args*/) -> pkpy::PyVar
				{
					return py_vm->None;
				});
		}, pkpy::VM::tp_object, true);
}
