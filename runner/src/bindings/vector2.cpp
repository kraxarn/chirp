#include "bindings.hpp"

#include <chirp/format.hpp>
#include <chirp/log.hpp>
#include <chirp/vector2.hpp>

using vector2 = chirp::vector2<pkpy::PyVar>;

void bind_magic_method(pkpy::VM *pkpy_vm, const pkpy::PyVar type, const char *signature, const pkpy::StrName &method)
{
	pkpy_vm->bind(type, signature, [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
	{
		const auto &self = pkpy::py_cast<vector2 &>(py_vm, args[0]);
		const auto &other = args[1];

		pkpy::PyVar other_x;
		pkpy::PyVar other_y;

		const auto &self_var = py_var(py_vm, self);
		if (is_type(other, self_var->type))
		{
			const auto &other_vec = pkpy::py_cast<vector2 &>(py_vm, other);
			other_x = other_vec.x();
			other_y = other_vec.y();
		}
		else
		{
			other_x = other;
			other_y = other;
		}

		const auto method = pkpy::lambda_get_userdata<pkpy::StrName>(args.begin());
		auto *result_x = py_vm->call_method(self.x(), method, other_x);
		auto *result_y = py_vm->call_method(self.y(), method, other_y);

		if (result_x == py_vm->NotImplemented || result_y == py_vm->NotImplemented)
		{
			chirp::log::fatal("invalid operation");
		}

		return py_var(py_vm, vector2(result_x, result_y));
	}, method);
}

void bind_vector2(pkpy::VM *py_vm, pkpy::PyVar mod)
{
	py_vm->register_user_class<vector2>(mod, "Vector2",
		[](pkpy::VM *pkpy_vm, pkpy::PyVar /*mod*/, const pkpy::PyVar type)
		{
			pkpy_vm->bind(type, "__init__(self, x, y)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				auto &self = pkpy::py_cast<vector2 &>(py_vm, args[0]);
				self = vector2(args[1], args[2]);
				return py_vm->None;
			});

			pkpy_vm->bind(type, "__str__(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<vector2 &>(py_vm, args[0]);

				pkpy::Dict dict(py_vm);
				dict.set(py_var(py_vm, "x"), self.x());
				dict.set(py_var(py_vm, "y"), self.y());

				return py_var(py_vm, py_vm->py_str(py_var(py_vm, dict)));
			});

			bind_magic_method(pkpy_vm, type, "__add__(self, other)", "__add__");
			bind_magic_method(pkpy_vm, type, "__sub__(self, other)", "__sub__");
			bind_magic_method(pkpy_vm, type, "__mul__(self, other)", "__mul__");
			bind_magic_method(pkpy_vm, type, "__truediv__(self, other)", "__truediv__");
			bind_magic_method(pkpy_vm, type, "__floordiv__(self, other)", "__floordiv__");

			pkpy_vm->bind(type, "x(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<vector2 &>(py_vm, args[0]);
				return self.x();
			});

			pkpy_vm->bind(type, "y(self)", [](pkpy::VM *py_vm, const pkpy::ArgsView args) -> pkpy::PyVar
			{
				const auto &self = pkpy::py_cast<vector2 &>(py_vm, args[0]);
				return self.y();
			});
		}, pkpy::VM::tp_object, true);
}
