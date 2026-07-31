#include "input.h"
#include "array.h"
#include "inputconfig.h"
#include "logcategory.h"
#include "map.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

typedef struct
{
	// Keyboard
	SDL_Keycode *keycodes;

	// Mouse button
	SDL_MouseButtonFlags mouse_button;

	// Gamepad axis
	SDL_GamepadAxis gamepad_axis;
	float deadzone;

	// Gamepad button
	SDL_GamepadButton gamepad_button;

	// Gamepad button label
	SDL_GamepadButtonLabel gamepad_button_label;
} input_map_t;

static void update_keyboard_event(const input_t input, const SDL_KeyboardEvent event)
{
	// Events get repeatedly triggered when key is held down
	const input_state_t state = map_get(input.key_map, event.key, STATE_UP);
	if (state != STATE_UP && event.down)
	{
		return;
	}

	map_set(input.key_map, event.key, event.down ? STATE_PRESSED : STATE_UP);
}

static void update_mouse_button_event(const input_t input, const SDL_MouseButtonEvent event)
{
	map_set(input.button_map, event.button, event.down ? STATE_PRESSED : STATE_UP);
}

static void update_gamepad_button_event(const input_t input, const SDL_GamepadButtonEvent event)
{
	SDL_Gamepad *gamepad = SDL_GetGamepadFromID(event.which);
	if (gamepad == nullptr)
	{
		SDL_LogError(LOG_CATEGORY_INPUT, "Unknown gamepad: %d", event.which);
		return;
	}

	const int index = SDL_GetGamepadPlayerIndex(gamepad);
	if (index < 0 || index >= input_gamepad_count)
	{
		SDL_LogError(LOG_CATEGORY_INPUT, "Invalid player index: %d", index);
		return;
	}

	map_set(input.gamepad_button_maps[index], event.button,
		event.down ? STATE_PRESSED : STATE_UP);

	const SDL_GamepadButtonLabel label = SDL_GetGamepadButtonLabel(gamepad, event.button);
	if (label != SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN)
	{
		map_set(input.gamepad_label_maps[index], label,
			event.down ? STATE_PRESSED : STATE_UP);
	}
}

bool input_create(input_t *input)
{
	input->key_map = map_create();
	input->button_map = map_create();
	input->name_map = map_create();

	for (size_t i = 0; i < input_gamepad_count; i++)
	{
		input->gamepad_button_maps[i] = map_create();
		if (input->gamepad_button_maps[i] == 0)
		{
			return false;
		}
	}

	for (size_t i = 0; i < input_gamepad_count; i++)
	{
		input->gamepad_label_maps[i] = map_create();
		if (input->gamepad_label_maps[i] == 0)
		{
			return false;
		}
	}

	return (bool) (input->key_map != 0
		&& input->button_map != 0
		&& input->name_map != 0);
}

void input_destroy(const input_t input)
{
	SDL_DestroyProperties(input.key_map);
	SDL_DestroyProperties(input.button_map);
	SDL_DestroyProperties(input.name_map);

	for (size_t i = 0; i < input_gamepad_count; i++)
	{
		SDL_DestroyProperties(input.gamepad_button_maps[i]);
	}

	for (size_t i = 0; i < input_gamepad_count; i++)
	{
		SDL_DestroyProperties(input.gamepad_label_maps[i]);
	}
}

void input_update(const input_t input, const SDL_Event *event)
{
	switch (event->type)
	{
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			update_keyboard_event(input, event->key);
			break;

		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			update_mouse_button_event(input, event->button);
			break;

		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			update_gamepad_button_event(input, event->gbutton);
			break;

		default:
			break;
	}
}

static void input_map_cleanup([[maybe_unused]] void *userdata, void *value)
{
	const input_map_t *map = value;
	array_destroy(map->keycodes);
	SDL_free(value);
}

bool input_add(const input_t input, const char *name, const input_config_t config)
{
	if (map_contains(input.name_map, name))
	{
		return SDL_SetError("Property already exists");
	}

	input_map_t *map = SDL_malloc(sizeof(input_map_t));
	if (map == nullptr)
	{
		return false;
	}

	if (!SDL_SetPointerPropertyWithCleanup(input.name_map, name, map,
		input_map_cleanup, nullptr))
	{
		SDL_free(map);
		return false;
	}

	map->keycodes = nullptr;
	map->mouse_button = 0;
	map->gamepad_axis = SDL_GAMEPAD_AXIS_INVALID;
	map->deadzone = 0.F;
	map->gamepad_button = SDL_GAMEPAD_BUTTON_INVALID;
	map->gamepad_button_label = SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN;

	bool is_valid = false;

	if (config.keycodes != nullptr)
	{
		map->keycodes = config.keycodes;
		is_valid = true;
	}

	if (config.mouse_button > 0)
	{
		map->mouse_button = config.mouse_button;
		is_valid = true;
	}

	if (config.gamepad_button != SDL_GAMEPAD_BUTTON_INVALID)
	{
		map->gamepad_button = config.gamepad_button;
		is_valid = true;
	}

	if (config.gamepad_button_label != SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN)
	{
		map->gamepad_button_label = config.gamepad_button_label;
		is_valid = true;
	}

	if (!is_valid)
	{
		SDL_ClearProperty(input.name_map, name);
		return SDL_SetError("Unknown input mapping");
	}

	return true;
}

void input_gamepad_open(const SDL_JoystickID joystick_id)
{
	SDL_Gamepad *gamepad = SDL_OpenGamepad(joystick_id);
	if (gamepad == nullptr)
	{
		SDL_LogError(LOG_CATEGORY_INPUT, "Failed to open gamepad %u: %s",
			joystick_id, SDL_GetError());
	}

	const int player_index = SDL_GetGamepadPlayerIndex(gamepad);
	const char *name = SDL_GetGamepadName(gamepad);
	const SDL_GamepadType type = SDL_GetGamepadType(gamepad);

	SDL_LogInfo(LOG_CATEGORY_INPUT, "Gamepad connected (%d, %s): '%s'",
		player_index, SDL_GetGamepadStringForType(type), name);
}

void input_gamepad_close(const SDL_JoystickID joystick_id)
{
	SDL_Gamepad *gamepad = SDL_GetGamepadFromID(joystick_id);
	if (gamepad == nullptr)
	{
		SDL_LogError(LOG_CATEGORY_INPUT, "Failed to close gamepad %u: %s",
			joystick_id, SDL_GetError());
	}

	SDL_LogInfo(LOG_CATEGORY_INPUT, "Gamepad disconnected: '%s'",
		SDL_GetGamepadName(gamepad));

	SDL_CloseGamepad(gamepad);
}

input_state_t input_state(const input_t input, const char *name,
	const bool reset_pressed)
{
	const input_map_t *input_map = map_get(input.name_map, name, nullptr);
	if (input_map == nullptr)
	{
		SDL_LogWarn(LOG_CATEGORY_INPUT, "Unmapped input: %s", name);
		return STATE_UP;
	}

	if (input_map->keycodes != nullptr)
	{
		for (size_t i = 0; i < array_size(input_map->keycodes); i++)
		{
			const SDL_Keycode keycode = array_at(input_map->keycodes, i);
			const input_state_t state = map_get(input.key_map, keycode, STATE_UP);

			if (reset_pressed && state == STATE_PRESSED)
			{
				map_set(input.key_map, keycode, STATE_DOWN);
			}

			if (state != STATE_UP)
			{
				return state;
			}
		}
	}

	if (input_map->mouse_button > 0)
	{
		const SDL_MouseButtonFlags button = input_map->mouse_button;
		const input_state_t state = map_get(input.button_map, button, STATE_UP);

		if (reset_pressed && state == STATE_PRESSED)
		{
			map_set(input.button_map, button, STATE_DOWN);
		}

		if (state != STATE_UP)
		{
			return state;
		}
	}

	if (input_map->gamepad_button != SDL_GAMEPAD_BUTTON_INVALID)
	{
		// TODO: Don't assume player 1 (/0)
		constexpr size_t index = 0;
		const SDL_GamepadButton button = input_map->gamepad_button;
		const input_state_t state = map_get(input.gamepad_button_maps[index], button, STATE_UP);

		if (reset_pressed && state == STATE_PRESSED)
		{
			map_set(input.gamepad_button_maps[index], button, STATE_DOWN);
		}

		if (state != STATE_UP)
		{
			return state;
		}
	}

	if (input_map->gamepad_button_label != SDL_GAMEPAD_BUTTON_LABEL_UNKNOWN)
	{
		// TODO: Don't assume player 1 (/0)
		constexpr size_t index = 0;
		const SDL_GamepadButtonLabel label = input_map->gamepad_button_label;
		const input_state_t state = map_get(input.gamepad_label_maps[index], label, STATE_UP);

		if (reset_pressed && state == STATE_PRESSED)
		{
			map_set(input.gamepad_label_maps[index], label, STATE_DOWN);
		}

		if (state != STATE_UP)
		{
			return state;
		}
	}

	return STATE_UP;
}

bool input_is_pressed(const input_t input, const char *name)
{
	return input_state(input, name, true) == STATE_PRESSED;
}

bool input_is_down(const input_t input, const char *name)
{
	return input_state(input, name, true) != STATE_UP;
}
