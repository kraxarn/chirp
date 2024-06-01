#pragma once

namespace chirp
{
	class os
	{
	public:
		/**
		 * Engine is built in debug mode
		 */
		static auto is_debug() -> bool;

	private:
		os() = default;
	};
}
