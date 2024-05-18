#include "chirp/sound.hpp"

#include "raylib.h"

chirp::sound::sound(const std::vector<unsigned char> &data)
	: r_wave(new Wave()),
	r_sound(new Sound())
{
	*r_wave = LoadWaveFromMemory(".wav",
		data.data(), static_cast<int>(data.size()));

	*r_sound = LoadSoundFromWave(*r_wave);
}

chirp::sound::~sound()
{
	UnloadSound(*r_sound);
	UnloadWave(*r_wave);

	delete r_sound;
	delete r_wave;
}

void chirp::sound::play() const
{
	PlaySound(*r_sound);
}
