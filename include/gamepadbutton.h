#pragma once

#include <SDL3/SDL_gamepad.h>

[[nodiscard]]
const char *gamepad_button_name(SDL_GamepadButton gamepad_button);

[[nodiscard]]
SDL_GamepadButton gamepad_button_from_name(const char *name);
