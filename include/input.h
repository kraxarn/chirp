#pragma once

#include "inputconfig.h"
#include "map.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_stdinc.h>

static constexpr size_t input_gamepad_count = 4;

typedef struct
{
	// key -> input state
	map_t key_map;

	// mouse button -> input state
	map_t button_map;

	// gamepad button -> input state
	map_t gamepad_button_maps[input_gamepad_count];

	// gamepad button labels -> input state
	map_t gamepad_label_maps[input_gamepad_count];

	// input name -> input map
	map_t name_map;
} input_t;

typedef enum : Sint64
{
	STATE_UP,
	STATE_PRESSED,
	STATE_DOWN,
} input_state_t;

bool input_create(input_t *input);

void input_destroy(input_t input);

void input_update(input_t input, const SDL_Event *event);

bool input_add(input_t input, const char *name, input_config_t config);

void input_gamepad_open(SDL_JoystickID joystick_id);

void input_gamepad_close(SDL_JoystickID joystick_id);

[[nodiscard]]
input_state_t input_state(input_t input, const char *name, bool reset_pressed);

[[nodiscard]]
bool input_is_pressed(input_t input, const char *name);

[[nodiscard]]
bool input_is_down(input_t input, const char *name);
