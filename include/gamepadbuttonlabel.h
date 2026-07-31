#pragma once

#include <SDL3/SDL_gamepad.h>

[[nodiscard]]
const char *gamepad_button_label_name(SDL_GamepadButtonLabel label);

[[nodiscard]]
SDL_GamepadButtonLabel gamepad_button_label_from_name(const char *name);
