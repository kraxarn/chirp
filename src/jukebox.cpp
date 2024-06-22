#include "chirp/jukebox.hpp"
#include "chirp/log.hpp"

void chirp::jukebox::update(const chirp::scene &/*scene*/, const float /*delta*/)
{
	if (!current)
	{
		return;
	}

	current->update();
}

void chirp::jukebox::draw() const
{
}

void chirp::jukebox::insert(const asset<music> &music)
{
	if (current)
	{
		current->stop();
	}

	current = music;
	current->set_volume(volume);
}

auto chirp::jukebox::empty() const -> bool
{
	return !current;
}

void chirp::jukebox::set_volume(const float value)
{
	volume = value;

	if (current)
	{
		current->set_volume(value);
	}
}

void chirp::jukebox::play() const
{
	if (!current)
	{
		chirp::log::error("Nothing in jukebox to play");
		return;
	}

	current->play();
}

void chirp::jukebox::stop() const
{
	if (!current)
	{
		chirp::log::error("Nothing in jukebox to stop");
		return;
	}

	current->stop();
}
