#pragma once

#include <chirp/format.hpp>
#include <chirp/loglevel.hpp>

namespace chirp
{
	class log
	{
	public:
		template<typename Format, typename Arg, typename... Args>
		static void info(const Format &fmt, const Arg &arg, Args &&... args)
		{
			info(fmt::format(fmt, arg, args...));
		}

		template<typename Format>
		static void info(const Format &fmt)
		{
			message(log_level::info, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			warn(fmt::format(fmt, arg, args...));
		}

		template<typename Format>
		static void warn(const Format &fmt)
		{
			message(log_level::warning, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			error(fmt::format(fmt, arg, args...));
		}

		template<typename Format>
		static void error(const Format &fmt)
		{
			message(log_level::error, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void fatal(const Format &fmt, const Arg &arg, Args &&... args)
		{
			fatal(fmt::format(fmt, arg, args...));
		}

		template<typename Format>
		static void fatal(const Format &fmt)
		{
			message(log_level::fatal, fmt);
		}

	private:
		static void message(log_level level, const std::string &message);
	};
}
