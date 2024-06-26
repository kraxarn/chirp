#include "bindings.hpp"

#include <chirp/log.hpp>

void bind_log(pkpy::VM *py_vm)
{
	auto *log_mod = py_vm->new_module("log", "chirp");

	py_vm->bind(log_mod, "info(message)", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto message = pkpy::py_cast<pkpy::Str>(pkpy_vm, args[0]);
		chirp::log::info(message.str());
		return pkpy_vm->None;
	});

	py_vm->bind(log_mod, "warn(message)", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto message = pkpy::py_cast<pkpy::Str>(pkpy_vm, args[0]);
		chirp::log::warn(message.str());
		return pkpy_vm->None;
	});

	py_vm->bind(log_mod, "error(message)", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto message = pkpy::py_cast<pkpy::Str>(pkpy_vm, args[0]);
		chirp::log::error(message.str());
		return pkpy_vm->None;
	});

	py_vm->bind(log_mod, "fatal(message)", [](pkpy::VM *pkpy_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto message = pkpy::py_cast<pkpy::Str>(pkpy_vm, args[0]);
		chirp::log::fatal(message.str());
		return pkpy_vm->None;
	});
}
