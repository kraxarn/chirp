#pragma once

#include "loglevel.hpp"
#include <string>

namespace chirp
{
	/**
	 * A logger for engine warnings and errors
	 */
	class logger
	{
	public:
		/**
		 * Log a message
		 * @param level Level type
		 * @param message Message content
		 */
		virtual void log(log_level level, const std::string &message) = 0;

		/**
		 * Initializes logger
		 */
		static void init();

		/**
		 * Get custom logger, if any
		 * @return Logger
		 */
		static auto get() -> logger *;

		/**
		 * Set a custom logger
		 * @param logger Custom logger to use
		 */
		static void set(logger &logger);

	private:
		/**
		 * Custom logger instance
		 */
		static logger *instance;
	};
}
