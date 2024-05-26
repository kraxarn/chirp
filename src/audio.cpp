#include "chirp/audio.hpp"

#include "raylib.h"

chirp::audio::audio()
{
	InitAudioDevice();
}

chirp::audio::~audio()
{
	CloseAudioDevice();
}
