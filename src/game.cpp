#include "chirp/game.hpp"
#include "chirp/colors.hpp"
#include "chirp/logger.hpp"
#include "chirp/monitor.hpp"

#include "raylib.h"

chirp::game::game()
	: running(false),
	target_fps(0),
	clear_color(colors::black())
{
	chirp::logger::init();
	InitWindow(default_width, default_height, default_title);
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	// TODO: Use vsync instead by default
	const auto monitor_index = chirp::monitor::current();
	set_target_fps(chirp::monitor::refresh_rate(monitor_index));

	SetExitKey(KEY_NULL);
}

chirp::game::~game()
{
	CloseWindow();
}

void chirp::game::run()
{
	running = true;
	while (running)
	{
		if (scenes().empty())
		{
			running = false;
			break;
		}

		auto &current_scene = scenes().peek();
		current_scene.update(GetFrameTime());

		BeginDrawing();

		ClearBackground({
			clear_color.red(),
			clear_color.green(),
			clear_color.blue(),
			clear_color.alpha(),
		});

		current_scene.draw();

		EndDrawing();

		if (WindowShouldClose())
		{
			running = false;
		}

		scenes().clear_queue();
	}
}

void chirp::game::exit()
{
	running = false;
}

auto chirp::game::get_target_fps() const -> int
{
	return target_fps;
}

void chirp::game::set_target_fps(int fps)
{
	target_fps = fps;
	SetTargetFPS(fps);
}

auto chirp::game::get_clear_color() const -> color
{
	return clear_color;
}

void chirp::game::set_clear_color(const color &color)
{
	clear_color = color;
}

auto chirp::game::window() const -> chirp::window
{
	return chirp::window();
}

auto chirp::game::scenes() -> chirp::scene_manager &
{
	return scene_manager;
}
