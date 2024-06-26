#include "chirp/logger.hpp"
#include "chirp/format.hpp"

#include "raylib.h"

#include <iostream>

chirp::logger *chirp::logger::instance = nullptr;

void log(const chirp::log_level level, const std::string &message)
{
	auto *logger = chirp::logger::get();
	if (logger != nullptr)
	{
		logger->log(level, message);
		return;
	}

	switch (level)
	{
		case chirp::log_level::info:
			std::cout << "[inf] " << message << '\n';
			break;

		case chirp::log_level::warning:
			std::cerr << "[wrn] " << message << '\n';
			break;

		case chirp::log_level::error:
			std::cerr << "[err] " << message << '\n';
			break;

		case chirp::log_level::fatal:
			std::cerr << "[ftl] " << message << '\n';
			break;

		default:
			std::cout << "[msg] " << message << '\n';
			break;
	}
}

void log_callback(int msg_type, const char *text, va_list args)
{
	chirp::log_level level;
	switch (msg_type)
	{
		case LOG_INFO:
			level = chirp::log_level::info;
			break;

		case LOG_ERROR:
			level = chirp::log_level::error;
			break;

		case LOG_WARNING:
			level = chirp::log_level::warning;
			break;

		case LOG_FATAL:
			level = chirp::log_level::fatal;
			break;

		default:
			log(chirp::log_level::warning, chirp::format("Unknown log level: {}", msg_type));
			return;
	}

	va_list args_copy;
	va_copy(args_copy, args);
	const auto len = vsnprintf(nullptr, 0, text, args_copy);
	std::string message(len + 1, '\0');
	const auto written = vsnprintf(message.data(), len + 1, text, args);
	message.resize(written);

	log(level, message);

	if (level == chirp::log_level::fatal)
	{
		// We could show a crash dialog or something here at some point
		std::exit(1);
	}
}

void chirp::logger::init()
{
	SetTraceLogCallback(log_callback);
}

auto chirp::logger::get() -> logger *
{
	return instance;
}

void chirp::logger::set(chirp::logger &logger)
{
	instance = &logger;
}
