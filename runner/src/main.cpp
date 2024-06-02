#include "bindings/bindings.hpp"

#include <chirp/logger.hpp>

#include <pocketpy.h>

#include <fstream>
#include <sstream>

auto main(int argc, char **argv) -> int
{
	chirp::logger::init();

	auto *py_vm = new pkpy::VM();

	auto *root_object = py_vm->new_module("chirp");

	bind_game(py_vm, root_object);

	if (argc > 1)
	{
		std::ifstream file(argv[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();
		py_vm->exec(buffer.str());
	}

	delete py_vm;
	return 0;
}
