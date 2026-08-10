#include "chirp/gamepadaxis.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

static const char *names[] = {
	[SDL_GAMEPAD_AXIS_LEFTX] = "LeftX",
	[SDL_GAMEPAD_AXIS_LEFTY] = "LeftY",
	[SDL_GAMEPAD_AXIS_RIGHTX] = "RightX",
	[SDL_GAMEPAD_AXIS_RIGHTY] = "RightY",
	[SDL_GAMEPAD_AXIS_LEFT_TRIGGER] = "LeftTrigger",
	[SDL_GAMEPAD_AXIS_RIGHT_TRIGGER] = "RightTrigger",
};

SDL_GamepadAxis gamepad_axis_from_name(const char *name)
{
	for (size_t i = 0; i < SDL_arraysize(names); i++)
	{
		if (SDL_strcasecmp(name, names[i]) == 0)
		{
			return i;
		}
	}

	return SDL_GAMEPAD_AXIS_INVALID;
}
