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
		 * Set the title of the main window, if avaialble
		 */
		void set_title(const std::string &title) const;

		/**
		 * Get the current size of the main window
		 */
		[[nodiscard]]
		auto get_size() const -> chirp::vector2i;

		/**
		 * Set the window size, resizing it if needed
		 */
		void set_size(const chirp::vector2i &size) const;
	};
}
