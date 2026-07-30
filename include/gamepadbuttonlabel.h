#pragma once

#include <SDL3/SDL_gamepad.h>

[[nodiscard]]
SDL_GamepadButtonLabel gamepad_button_label_from_name(const char *name);
