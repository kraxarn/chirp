#pragma once

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

typedef struct
{
	SDL_Keycode *keycodes;
	SDL_MouseButtonFlags mouse_button;
	SDL_GamepadAxis gamepad_axis;
	float gamepad_axis_range[2];
} input_config_t;
