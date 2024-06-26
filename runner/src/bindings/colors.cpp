#include "bindings.hpp"

#include <chirp/colors.hpp>

void bind_colors(pkpy::VM *py_vm)
{
	auto *log_mod = py_vm->new_module("colors", "chirp");

	py_vm->bind(log_mod, "black()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView) -> pkpy::PyVar
	{
		return py_var(pkpy_vm, chirp::colors::black());
	});

	py_vm->bind(log_mod, "white()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView) -> pkpy::PyVar
	{
		return py_var(pkpy_vm, chirp::colors::white());
	});

	py_vm->bind(log_mod, "red()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView) -> pkpy::PyVar
	{
		return py_var(pkpy_vm, chirp::colors::red());
	});

	py_vm->bind(log_mod, "green()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView) -> pkpy::PyVar
	{
		return py_var(pkpy_vm, chirp::colors::green());
	});

	py_vm->bind(log_mod, "blue()", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView) -> pkpy::PyVar
	{
		return py_var(pkpy_vm, chirp::colors::blue());
	});
}
