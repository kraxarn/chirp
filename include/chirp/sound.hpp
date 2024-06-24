#pragma once

#include <vector>

struct Sound;
struct Wave;

namespace chirp
{
	class sound
	{
	public:
		explicit sound(const std::vector<unsigned char> &data);
		~sound();

		void play() const;

		[[nodiscard]]
		auto is_playing() const -> bool;

	private:
		Wave *r_wave;
		Sound *r_sound;
	};
}
