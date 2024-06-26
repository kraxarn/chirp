#include "bindings.hpp"

#include <chirp/rectangle.hpp>

using rectangle = chirp::rectangle<pkpy::PyVar>;

void bind_rectangle(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<rectangle>(mod, "Rectangle",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "__init__(self, x, y, width, height)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);
					self = rectangle(args[1], args[2], args[3], args[4]);
					return py_vm->None;
				});

			pkpy_vm->bind(type, "__str__(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);

				pkpy::Dict dict(py_vm);
				dict.set(py_var(py_vm, "x"), self.x());
				dict.set(py_var(py_vm, "y"), self.y());
				dict.set(py_var(py_vm, "width"), self.width());
				dict.set(py_var(py_vm, "height"), self.height());

				return py_var(py_vm, py_vm->py_str(py_var(py_vm, dict)));
			});

			pkpy_vm->bind(type, "x(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);
				return self.x();
			});

			pkpy_vm->bind(type, "y(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);
				return self.y();
			});

			pkpy_vm->bind(type, "width(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);
				return self.width();
			});

			pkpy_vm->bind(type, "height(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<rectangle &>(py_vm, args[0]);
				return self.height();
			});
		}, pkpy::VM::tp_object, true);
}
