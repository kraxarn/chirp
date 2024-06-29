#include "bindings/bindings.hpp"

#include <chirp/logger.hpp>

#include <pocketpy.h>
#include <pybind11/embed.h>

#include <fstream>
#include <sstream>
#include <chirp/log.hpp>

PYBIND11_EMBEDDED_MODULE(chirp, module)
{
	try
	{
		bind_color(module);
		bind_font(module);
		bind_game(module);
		bind_image(module);
		bind_jukebox(module);
		bind_level(module);
		bind_os(module);
		bind_scene(module);
		bind_scene_manager(module);
		bind_vector2(module);
		bind_window(module);
	}
	catch (const pkpy::Exception &e)
	{
		chirp::log::fatal("Binding failed: {}", e.msg.str());
	}
}

auto main(int argc, char **argv) -> int
{
	chirp::logger::init();

	pybind11::scoped_interpreter guard{};

	pybind11::module_::import("chirp");

	if (argc > 1)
	{
		std::ifstream file(argv[1]);
		std::stringstream buffer;
		buffer << file.rdbuf();

		try
		{
			pybind11::exec(buffer.str());
		}
		catch (const pkpy::Exception &e)
		{
			chirp::log::fatal("Interpreter error: {}", e.msg.str());
		}
	}

	return 0;
}
