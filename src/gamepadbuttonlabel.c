#include "gamepadbuttonlabel.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_stdinc.h>

static const char *names[] = {
	[SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN] = "Unknown",
	[SDL_GAMEPAD_BUTTON_LABEL_A] = "A",
	[SDL_GAMEPAD_BUTTON_LABEL_B] = "B",
	[SDL_GAMEPAD_BUTTON_LABEL_X] = "X",
	[SDL_GAMEPAD_BUTTON_LABEL_Y] = "Y",
	[SDL_GAMEPAD_BUTTON_LABEL_CROSS] = "Cross",
	[SDL_GAMEPAD_BUTTON_LABEL_CIRCLE] = "Circle",
	[SDL_GAMEPAD_BUTTON_LABEL_SQUARE] = "Square",
	[SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE] = "Triangle",
};

SDL_GamepadButtonLabel gamepad_button_label_from_name(const char *name)
{
	for (size_t i = 0; i < SDL_arraysize(names); i++)
	{
		if (SDL_strcasecmp(name, names[i]) == 0)
		{
			return i;
		}
	}

	return SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN;
}
