#pragma once

namespace chirp
{
	/**
	 * Level of logging
	 */
	enum class log_level
	{
		/**
		 * Execution information
		 */
		info = 1 << 0,

		/**
		 * Warnings, recoverable errors
		 */
		warning = 1 << 1,

		/**
		 * Error, unrecoverable errors
		 */
		error = 1 << 2,

		/**
		 * Fatal, abort program
		 */
		fatal = 1 << 3,
	};
}
