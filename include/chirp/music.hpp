#pragma once

#include <string>

struct Music;

namespace chirp
{
	class music
	{
	public:
		explicit music(const std::vector<unsigned char> &data, std::string name);
		~music();

		void play() const;

		[[nodiscard]]
		auto is_playing() const -> bool;

		void stop() const;

		void pause() const;

		void resume() const;

		[[nodiscard]]
		auto length() const -> float;

		[[nodiscard]]
		auto played() const -> float;

		void update() const;

		[[nodiscard]]
		auto name() const -> const std::string &;

	private:
		Music *r_music;
		std::string file_name;

		static auto guess_file_type(const std::vector<unsigned char> &data) -> const char *;
	};
}
