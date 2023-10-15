#pragma once

#include "vector2.hpp"

namespace chirp
{
	/**
	 * Monitor information
	 */
	class monitor
	{
	public:
		/**
		 * Number of monitors
		 * @return Count
		 */
		static auto count() -> int;

		/**
		 * Current monitor
		 * @return Index
		 */
		static auto current() -> int;

		/**
		 * Position of monitor at index
		 * @param index Monitor index
		 * @return Position
		 */
		static auto position(int index) -> vector2f;

		/**
		 * Siz eof monitor at index
		 * @param index Monitor index
		 * @return Size in pixels
		 */
		static auto size(int index) -> vector2i;

		/**
		 * Refresh rate of monitor at index
		 * @param index Monitor index
		 * @return Refresh rate in hz
		 */
		static auto refresh_rate(int index) -> int;

	private:
		monitor() = default;
	};
}
