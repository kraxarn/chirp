#include "bindings.hpp"

#include <chirp/game.hpp>

void bind_game(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<chirp::game>(mod, "Game",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "run(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::game &>(py_vm, args[0]);
					self.run();
					return py_vm->None;
				});

			pkpy_vm->bind(type, "exit(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::game &>(py_vm, args[0]);
					self.exit();
					return py_vm->None;
				});

			pkpy_vm->bind(type, "get_target_fps(self)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					const auto &self = pkpy::py_cast<chirp::game &>(py_vm, args[0]);
					return py_var(py_vm, self.get_target_fps());
				});

			pkpy_vm->bind(type, "set_target_fps(self, fps)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::game &>(py_vm, args[0]);
					auto fps = pkpy::py_cast<int>(py_vm, args[1]);
					self.set_target_fps(fps);
					return py_vm->None;
				});
		}, pkpy::VM::tp_object, true);
}
