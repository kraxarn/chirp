#pragma once

#include <chirp/color.hpp>

namespace chirp
{
	class colors
	{
	public:
		/**
		 * Black (#000000)
		 * @return Color
		 */
		static auto black() -> color;

		/**
		 * White (#ffffff)
		 * @return Color
		 */
		static auto white() -> color;

		/**
		 * Red (#ff0000)
		 */
		static auto red() -> color;

		/**
		 * Green (#00ff00)
		 */
		static auto green() -> color;

		/**
		 * Blue (#0000ff)
		 */
		static auto blue() -> color;

	private:
		colors() = default;

		static auto min() -> unsigned char;
		static auto max() -> unsigned char;
	};
}
