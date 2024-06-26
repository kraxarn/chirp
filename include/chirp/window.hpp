#pragma once

#include <chirp/vector2.hpp>

#include <string>

namespace chirp
{
	class window
	{
	public:
		explicit window() = default;

		/**
		 * Get the title of the main window, if available
		 */
		[[nodiscard]]
		auto get_title() const -> const std::string &;

		/**
		 * Set the title of the main window, if available
		 */
		void set_title(const std::string &title);

		/**
		 * Get the current size of the main window
		 */
		[[nodiscard]]
		auto get_size() const -> chirp::vector2i;

		/**
		 * Set the window size, resizing it if needed
		 */
		void set_size(const chirp::vector2i &size) const;

		/**
		 * Window got resized during the previous frame
		 */
		[[nodiscard]]
		auto is_resized() const -> bool;

	private:
		std::string title;
	};
}
