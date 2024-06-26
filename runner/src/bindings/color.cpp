#include "bindings.hpp"

#include <chirp/color.hpp>
#include <chirp/format.hpp>

void bind_color(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<chirp::color>(mod, "Color",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "__init__(self, red, green, blue, alpha=255)",
				[](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
				{
					auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);

					const auto red = pkpy::py_cast<int>(py_vm, args[1]);
					const auto green = pkpy::py_cast<int>(py_vm, args[2]);
					const auto blue = pkpy::py_cast<int>(py_vm, args[3]);
					const auto alpha = pkpy::py_cast<int>(py_vm, args[4]);

					self = chirp::color(red, green, blue, alpha);
					return py_vm->None;
				});

			pkpy_vm->bind(type, "__str__(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);

				pkpy::Dict dict(py_vm);
				dict.set(py_var(py_vm, "red"), py_var(py_vm, self.red()));
				dict.set(py_var(py_vm, "green"), py_var(py_vm, self.green()));
				dict.set(py_var(py_vm, "blue"), py_var(py_vm, self.blue()));
				dict.set(py_var(py_vm, "alpha"), py_var(py_vm, self.alpha()));

				return py_var(py_vm, py_vm->py_str(py_var(py_vm, dict)));
			});

			pkpy_vm->bind(type, "red(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);
				return py_var(py_vm, self.red());
			});

			pkpy_vm->bind(type, "green(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);
				return py_var(py_vm, self.green());
			});

			pkpy_vm->bind(type, "blue(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);
				return py_var(py_vm, self.blue());
			});

			pkpy_vm->bind(type, "alpha(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<chirp::color &>(py_vm, args[0]);
				return py_var(py_vm, self.alpha());
			});
		}, pkpy::VM::tp_object, true);
}
