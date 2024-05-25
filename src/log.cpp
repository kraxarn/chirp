#include "chirp/log.hpp"

#include "raylib.h"

void chirp::log::message(const log_level level, const std::string &message)
{
	TraceLogLevel log_level;

	switch (level)
	{
		case log_level::info:
			log_level = LOG_INFO;
			break;

		case log_level::warning:
			log_level = LOG_WARNING;
			break;

		case log_level::error:
			log_level = LOG_ERROR;
			break;

		case log_level::fatal:
			log_level = LOG_FATAL;
			break;

		default:
			log_level = LOG_NONE;
			break;
	}

	TraceLog(log_level, message.c_str());
}
