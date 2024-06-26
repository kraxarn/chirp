#include "bindings.hpp"

#include <chirp/scenemanager.hpp>

void bind_scene_manager(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<chirp::scene_manager>(mod, "SceneManager",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "push(self, scene)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::scene_manager &>(py_vm, args[0]);
					auto scene_type = args[1];
					py_vm->call(scene_type, pkpy::py_var(py_vm, self.get_assets()));
					// TODO
					return py_vm->None;
				});

			pkpy_vm->bind(type, "queue_pop(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::scene_manager &>(py_vm, args[0]);
					self.queue_pop();
					return py_vm->None;
				});

			pkpy_vm->bind(type, "peek(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					const auto &self = pkpy::py_cast<chirp::scene_manager &>(py_vm, args[0]);
					return py_var(py_vm, self.peek());
				});
		}, pkpy::VM::tp_object, true);
}
