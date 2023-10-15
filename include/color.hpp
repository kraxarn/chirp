#pragma once

namespace chirp
{
	class color
	{
	public:
		/**
		 * Construct a new color
		 * @param red Red
		 * @param green Green
		 * @param blue Blue
		 * @param alpha Alpha (opacity)
		 */
		color(const unsigned char red, const unsigned char green,
			const unsigned char blue, const unsigned char alpha = 255);

		/**
		 * Red component
		 * @return Amount of red
		 */
		[[nodiscard]]
		auto red() const -> unsigned char;

		/**
		 * Green component
		 * @return Amount of green
		 */
		[[nodiscard]]
		auto green() const -> unsigned char;

		/**
		 * Blue component
		 * @return Amount of blue
		 */
		[[nodiscard]]
		auto blue() const -> unsigned char;

		/**
		 * Alpha component
		 * @return Amount of alpha
		 */
		[[nodiscard]]
		auto alpha() const -> unsigned char;

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

	private:
		/**
		 * Red component
		 */
		const unsigned char red_val;

		/**
		 * Green component
		 */
		const unsigned char green_val;

		/**
		 * Blue component
		 */
		const unsigned char blue_val;

		/**
		 * Alpha component
		 */
		const unsigned char alpha_val;
	};
}
