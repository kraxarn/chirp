#include "chirp/gamepadbutton.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

/*
 * There is technically already a SDL_GetGamepadButtonFromString,
 * but it expects "south", "east" etc. as "a", "b", etc. which
 * we want to be able to differentiate between, so just
 * roll our own parser for the time being.
 * TODO: Maybe roll our own parsers for everything for consistency?
 */

static const char *names[] = {
	[SDL_GAMEPAD_BUTTON_SOUTH] = "South",
	[SDL_GAMEPAD_BUTTON_EAST] = "East",
	[SDL_GAMEPAD_BUTTON_WEST] = "West",
	[SDL_GAMEPAD_BUTTON_NORTH] = "North",
	[SDL_GAMEPAD_BUTTON_BACK] = "Back", // I remember when this was called "select" :(
	[SDL_GAMEPAD_BUTTON_GUIDE] = "Guide",
	[SDL_GAMEPAD_BUTTON_START] = "Start",
	[SDL_GAMEPAD_BUTTON_LEFT_STICK] = "LeftStick",
	[SDL_GAMEPAD_BUTTON_RIGHT_STICK] = "RightStick",
	[SDL_GAMEPAD_BUTTON_LEFT_SHOULDER] = "LeftShoulder",
	[SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER] = "RightShoulder",
	[SDL_GAMEPAD_BUTTON_DPAD_UP] = "DpadUp",
	[SDL_GAMEPAD_BUTTON_DPAD_DOWN] = "DpadDown",
	[SDL_GAMEPAD_BUTTON_DPAD_LEFT] = "DpadLeft",
	[SDL_GAMEPAD_BUTTON_DPAD_RIGHT] = "DpadRight",
	[SDL_GAMEPAD_BUTTON_MISC1] = "Misc1",
	[SDL_GAMEPAD_BUTTON_RIGHT_PADDLE1] = "RightPaddle1",
	[SDL_GAMEPAD_BUTTON_LEFT_PADDLE1] = "LeftPaddle1",
	[SDL_GAMEPAD_BUTTON_RIGHT_PADDLE2] = "RightPaddle2",
	[SDL_GAMEPAD_BUTTON_LEFT_PADDLE2] = "LeftPaddle2",
	[SDL_GAMEPAD_BUTTON_TOUCHPAD] = "Touchpad",
	[SDL_GAMEPAD_BUTTON_MISC2] = "Misc2",
	[SDL_GAMEPAD_BUTTON_MISC3] = "Misc3",
	[SDL_GAMEPAD_BUTTON_MISC4] = "Misc4",
	[SDL_GAMEPAD_BUTTON_MISC5] = "Misc5",
	[SDL_GAMEPAD_BUTTON_MISC6] = "Misc6",
};

const char *gamepad_button_name(const SDL_GamepadButton button)
{
	SDL_assert(button > SDL_GAMEPAD_BUTTON_INVALID);
	SDL_assert(button < SDL_GAMEPAD_BUTTON_COUNT);

	return names[button];
}

SDL_GamepadButton gamepad_button_from_name(const char *name)
{
	for (size_t i = 0; i < SDL_arraysize(names); i++)
	{
		if (SDL_strcasecmp(name, names[i]) == 0)
		{
			return i;
		}
	}

	return SDL_GAMEPAD_BUTTON_INVALID;
}
