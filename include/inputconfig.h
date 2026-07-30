#pragma once

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

// TODO: Maybe just remove this and use input_map_t?
typedef struct
{
	SDL_Keycode *keycodes;
	SDL_MouseButtonFlags mouse_button;
	SDL_GamepadAxis gamepad_axis;
	float deadzone;
	SDL_GamepadButton gamepad_button;
	SDL_GamepadButtonLabel gamepad_button_label;
} input_config_t;

[[nodiscard]]
input_config_t input_config_default();
