#include "input.h"
#include "array.h"
#include "inputconfig.h"
#include "logcategory.h"
#include "map.h"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>

typedef enum : Sint64
{
	STATE_UP,
	STATE_PRESSED,
	STATE_DOWN,
} input_state_t;

typedef enum : Uint8
{
	TYPE_UNKNOWN,
	TYPE_KEYBOARD,
	TYPE_MOUSE_BUTTON,
} input_type_t;

typedef struct
{
	input_type_t type;

	union
	{
		SDL_Keycode *keycodes;
		SDL_MouseButtonFlags mouse_button;
	};
} input_map_t;

static void update_keyboard_event(const input_t *input, const SDL_KeyboardEvent event)
{
	// Events get repeatedly triggered when key is held down
	const input_state_t state = map_get(input->key_map, event.key, STATE_UP);
	if (state != STATE_UP && event.down)
	{
		return;
	}

	map_set(input->key_map, event.key, event.down ? STATE_PRESSED : STATE_UP);
}

static void update_mouse_button_event(const input_t *input, const SDL_MouseButtonEvent event)
{
	map_set(input->button_map, event.button, event.down ? STATE_PRESSED : STATE_UP);
}

bool input_create(input_t *input)
{
	input->key_map = map_create();
	input->button_map = map_create();
	input->name_map = map_create();

	return (bool) (input->key_map != 0
		&& input->button_map != 0
		&& input->name_map != 0);
}

void input_update(const input_t *input, const SDL_Event *event)
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

		default:
			break;
	}
}

static void input_map_cleanup([[maybe_unused]] void *userdata, void *value)
{
	const input_map_t *map = value;
	if (map->type == TYPE_KEYBOARD)
	{
		array_destroy(map->keycodes);
	}

	SDL_free(value);
}

bool input_add(const input_t *input, const char *name, const input_config_t config)
{
	if (map_contains(input->name_map, name))
	{
		return SDL_SetError("Property already exists");
	}

	input_map_t *map = SDL_malloc(sizeof(input_map_t));

	if (!SDL_SetPointerPropertyWithCleanup(input->name_map, name, map,
		input_map_cleanup, nullptr))
	{
		SDL_free(map);
		return false;
	}

	if (config.keycodes != nullptr)
	{
		map->type = TYPE_KEYBOARD;
		map->keycodes = config.keycodes;
	}
	else if (config.mouse_button > 0)
	{
		map->type = TYPE_MOUSE_BUTTON;
		map->mouse_button = config.mouse_button;
	}
	else
	{
		SDL_ClearProperty(input->name_map, name);
		return SDL_SetError("Unknown input mapping");
	}

	return true;
}

[[nodiscard]]
static input_state_t input_state(const input_t *input, const char *name)
{
	const input_map_t *input_map = map_get(input->name_map, name, nullptr);
	if (input_map == nullptr)
	{
		SDL_LogWarn(LOG_CATEGORY_INPUT, "Unmapped input: %s", name);
		return STATE_UP;
	}

	if (input_map->type == TYPE_KEYBOARD)
	{
		for (size_t i = 0; i < array_size(input_map->keycodes); i++)
		{
			const SDL_Keycode keycode = array_at(input_map->keycodes, i);
			const input_state_t state = map_get(input->key_map, keycode, STATE_UP);

			if (state == STATE_PRESSED)
			{
				map_set(input->key_map, keycode, STATE_DOWN);
			}

			if (state != STATE_UP)
			{
				return state;
			}
		}

		return STATE_UP;
	}

	if (input_map->type == TYPE_MOUSE_BUTTON)
	{
		const SDL_MouseButtonFlags button = input_map->mouse_button;
		const input_state_t state = map_get(input->button_map, button, STATE_UP);

		if (state == STATE_PRESSED)
		{
			map_set(input->button_map, button, STATE_DOWN);
		}

		return state;
	}

	SDL_LogError(LOG_CATEGORY_INPUT, "Unknown input type: %d", input_map->type);
	return STATE_UP;
}

bool input_is_pressed(const input_t *input, const char *name)
{
	return input_state(input, name) == STATE_PRESSED;
}

bool input_is_down(const input_t *input, const char *name)
{
	return input_state(input, name) != STATE_UP;
}
