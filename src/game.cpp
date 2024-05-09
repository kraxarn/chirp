#include "game.hpp"
#include "monitor.hpp"
#include "logger.hpp"

#include "raylib.h"

chirp::game::game()
	: running(false),
	target_fps(0),
	clear_color(chirp::color::black())
{
	chirp::logger::init();
	InitWindow(default_width, default_height, default_title);

	const auto monitor_index = chirp::monitor::current();
	set_target_fps(chirp::monitor::refresh_rate(monitor_index));
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
		update(GetFrameTime());

		BeginDrawing();

		ClearBackground({
			clear_color.red(),
			clear_color.green(),
			clear_color.blue(),
			clear_color.alpha(),
		});

		draw();

		EndDrawing();

		if (WindowShouldClose())
		{
			running = false;
		}
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
