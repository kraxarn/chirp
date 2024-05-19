#pragma once

namespace chirp
{
	class random
	{
	public:
		/**
		 * Get a random number between min and max, inclusive
		 */
		static auto get(int min, int max) -> int;

	private:
		random() = default;
	};
}
