#include "chirp/inputconfig.h"

#include <SDL3/SDL_gamepad.h>

input_config_t input_config_default()
{
	return (input_config_t){
		.keycodes = nullptr,
		.mouse_button = 0,
		.gamepad_axis = SDL_GAMEPAD_AXIS_INVALID,
		.deadzone = 0.F, // TODO: Maybe default to 0.2 (like Godot) or something
		.gamepad_button = SDL_GAMEPAD_BUTTON_INVALID,
	};
}
