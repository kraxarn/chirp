#include "chirp/music.hpp"

#include "raylib.h"

#include <vector>

chirp::music::music(const std::vector<unsigned char> &data, std::string name)
	: r_music(new Music()),
	file_name(std::move(name)),
	volume(1.F)
{
	*r_music = LoadMusicStreamFromMemory(".xm",
		data.data(), static_cast<int>(data.size()));

	r_music->looping = true;
}

chirp::music::~music()
{
	if (is_playing())
	{
		stop();
	}

	UnloadMusicStream(*r_music);
	delete r_music;
}

void chirp::music::play() const
{
	PlayMusicStream(*r_music);
}

auto chirp::music::is_playing() const -> bool
{
	return IsMusicStreamPlaying(*r_music);
}

void chirp::music::stop() const
{
	StopMusicStream(*r_music);
}

void chirp::music::pause() const
{
	PauseMusicStream(*r_music);
}

void chirp::music::resume() const
{
	ResumeMusicStream(*r_music);
}

auto chirp::music::length() const -> float
{
	return GetMusicTimeLength(*r_music);
}

auto chirp::music::played() const -> float
{
	return GetMusicTimePlayed(*r_music);
}

void chirp::music::update() const
{
	UpdateMusicStream(*r_music);
}

auto chirp::music::name() const -> const std::string &
{
	return file_name;
}

auto chirp::music::get_volume() const -> float
{
	return volume;
}

void chirp::music::set_volume(const float value) const
{
	SetMusicVolume(*r_music, value);
}


auto chirp::music::guess_file_type(const std::vector<unsigned char> &data) -> const char *
{
	constexpr size_t xm_offset = 37;
	constexpr unsigned char xm_value = 0x1a;

	if (data.at(xm_offset) == xm_value)
	{
		return ".xm";
	}

	return "";
}
