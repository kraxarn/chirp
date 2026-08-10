#pragma once

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

typedef enum : char
{
	AXIS_UNKNOWN  = 0,
	AXIS_NEGATIVE = '-',
	AXIS_POSITIVE = '+',
} input_axis_sign_t;

typedef struct
{
	SDL_Keycode *keycodes;
	SDL_MouseButtonFlags mouse_button;
	SDL_GamepadAxis gamepad_axis;
	float deadzone;
	input_axis_sign_t gamepad_axis_sign;
	SDL_GamepadButton gamepad_button;
	SDL_GamepadButtonLabel gamepad_button_label;
} input_config_t;

[[nodiscard]]
input_config_t input_config_default();
