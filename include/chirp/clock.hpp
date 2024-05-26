#pragma once

namespace chirp
{
	class clock
	{
	public:
		/**
		 * Application FPS (frames per second)
		 */
		static auto fps() -> int;

		/**
		 * Seconds since last frame (delta)
		 */
		static auto delta() -> float;

		/**
		 * Seconds since application start
		 */
		static auto time() -> double;
	};
}
