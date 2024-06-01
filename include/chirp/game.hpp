#pragma once

#include "color.hpp"

namespace chirp
{
	/**
	 * The basics for any game, creates a window and updates the contents of it
	 */
	class game
	{
	public:
		/**
		 * Run game
		 */
		void run();

	protected:
		/**
		 * Load various game assets
		 */
		virtual void load_content() = 0;

		/**
		 * Update game logic
		 * @param delta Time in seconds since the last frame
		 * @note No guarantee how often it will be called
		 */
		virtual void update(float delta) = 0;

		/**
		 * Draw game content
		 */
		virtual void draw() = 0;

		/**
		 * Create a new window and context
		 */
		game();

		/**
		 * Close window and release resources
		 */
		~game();

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
	};
}
