#pragma once

#include <chirp/audio.hpp>
#include <chirp/color.hpp>
#include <chirp/scenemanager.hpp>
#include <chirp/window.hpp>

namespace chirp
{
	/**
	 * The basics for any game, creates a window and updates the contents of it
	 */
	class game
	{
	public:
		/**
		 * Create a new window and context
		 */
		game();

		/**
		 * Close window and release resources
		 */
		virtual ~game();

		/**
		 * Run game
		 */
		void run();

	protected:
		/**
		 * Exit game and close window
		 */
		void exit();

		/**
		 * Get target FPS
		 * @return FPS
		 */
		[[nodiscard]]
		auto get_target_fps() const -> int;

		/**
		 * Set target FPS
		 * @param fps FPS
		 * @note Defaults to monitor refresh rate
		 */
		void set_target_fps(int fps);

		/**
		 * \brief Get window clear color
		 * \return Color
		 */
		[[nodiscard]]
		auto get_clear_color() const -> color;

		/**
		 * \brief Set window clear color
		 * \param color Color
		 */
		void set_clear_color(const color &color);

		[[nodiscard]]
		auto window() const -> chirp::window;

		/**
		 * Get current scene manager
		 */
		auto scenes() -> scene_manager &;

	private:
		/**
		 * Default window width
		 */
		static constexpr int default_width = 1280;

		/**
		 * Default window height
		 */
		static constexpr int default_height = 720;

		/**
		 * Default window title
		 */
		static constexpr const char *default_title = "game window - chirp";

		/**
		 * Game is running
		 */
		bool running;

		/**
		 * Target FPS
		 */
		int target_fps;

		/**
		 * Clear color for each frame
		 */
		color clear_color;

		/**
		 * All scenes
		 */
		scene_manager scene_manager;

		/**
		 * Sound manager
		 */
		audio audio;
	};
}
