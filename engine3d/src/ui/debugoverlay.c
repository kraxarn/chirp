#include "ui/debugoverlay.h"
#include "camera.h"
#include "ecs.h"
#include "nkui.h"
#include "timestats.h"
#include "ecs/components.h"

#include "flecs.h"
#include "box3d/box3d.h"
#include "box3d/id.h"
#include "box3d/math_functions.h"
#include "chirp/ecs.h"
#include "chirp/gamepadbutton.h"
#include "chirp/gamepadbuttonlabel.h"
#include "chirp/input.h"
#include "chirp/logcategory.h"
#include "chirp/map.h"
#include "chirp/mousebutton.h"
#include "chirp/ecs/components.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

static void draw_camera_info(nk_context_t *ctx, const camera_t *camera)
{
	nk_label(ctx, "Camera", NK_TEXT_LEFT);
	nk_labelf(ctx, NK_TEXT_LEFT, "%-6.2f %-6.2f %-6.2f",
		camera->position.x, camera->position.y, camera->position.z);

	nk_label(ctx, "Target", NK_TEXT_LEFT);
	nk_labelf(ctx, NK_TEXT_LEFT, "%-6.2f %-6.2f %-6.2f",
		camera->target.x, camera->target.y, camera->target.z);
}

static void draw_physics_info(nk_context_t *ctx, const b3BodyId body_id)
{
	const b3Vec3 position = b3Body_GetPosition(body_id);
	const b3Vec3 velocity = b3Body_GetLinearVelocity(body_id);
	const b3Quat rotation = b3Body_GetRotation(body_id);

	nk_label(ctx, "Position", NK_TEXT_LEFT);
	nk_labelf(ctx, NK_TEXT_LEFT, "%-6.2f %-6.2f %-6.2f",
		position.x, position.y, position.z);

	nk_label(ctx, "Velocity", NK_TEXT_LEFT);
	nk_labelf(ctx, NK_TEXT_LEFT, "%-6.2f %-6.2f %-6.2f",
		velocity.x, velocity.y, velocity.z);

	nk_label(ctx, "Rotation", NK_TEXT_LEFT);
	nk_labelf(ctx, NK_TEXT_LEFT, "%-6.2f %-6.2f %-6.2f",
		rotation.v.x, rotation.v.y, rotation.v.z);
}

static void draw_render_config(nk_context_t *ctx, const float row_height)
{
	nk_layout_row(ctx, NK_DYNAMIC, row_height * 1.5F, 2, (float[]){0.4F, 0.6F});

	static int selected_frame_rate = 0;
	const int previous_frame_rate = selected_frame_rate;
	static const char *frame_rates[] = {
		"Default",
		"30  FPS",
		"60  FPS",
		"90  FPS",
		"120 FPS",
		"180 FPS",
		"240 FPS",
		"360 FPS",
	};

	nk_label(ctx, "Target FPS", NK_TEXT_LEFT);
	selected_frame_rate = nk_combo(ctx, frame_rates, SDL_arraysize(frame_rates),
		selected_frame_rate, (int) row_height, nk_vec2(100.F, 190.F));

	if (previous_frame_rate != selected_frame_rate)
	{
		static float values[] = {0.F, 30.F, 60.F, 90.F, 120.F, 180.F, 240.F, 360.F};
		ecs_set_target_fps(ecs_world(), values[selected_frame_rate]);
	}

	// Set in ecs/gpudevice
	static int selected_hdr = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
	static int selected_vsync = SDL_GPU_PRESENTMODE_VSYNC;

	static const char *hdr_modes[] = {
		[SDL_GPU_SWAPCHAINCOMPOSITION_SDR] = "SDR",
		[SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR] = "SDR (linear)",
		[SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR] = "HDR",
		[SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084] = "HDR10",
	};

	static const char *vsync_modes[] = {
		[SDL_GPU_PRESENTMODE_VSYNC] = "VSync",
		[SDL_GPU_PRESENTMODE_IMMEDIATE] = "Immediate",
		[SDL_GPU_PRESENTMODE_MAILBOX] = "Mailbox",
	};

	const int previous_hdr = selected_hdr;
	const int previous_vsync = selected_vsync;

	nk_label(ctx, "HDR", NK_TEXT_LEFT);
	selected_hdr = nk_combo(ctx, hdr_modes, SDL_arraysize(hdr_modes),
		selected_hdr, (int) row_height, nk_vec2(140.F, 160.F));

	nk_label(ctx, "VSync", NK_TEXT_LEFT);
	selected_vsync = nk_combo(ctx, vsync_modes, SDL_arraysize(vsync_modes),
		selected_vsync, (int) row_height, nk_vec2(130.F, 160.F));

	if (selected_hdr != previous_hdr
		|| selected_vsync != previous_vsync)
	{
		SDL_Window *window = *(SDL_Window**) ecs_get_id(ecs_world(),
			ecs_singleton(EcsWindow));

		SDL_GPUDevice *device = *(SDL_GPUDevice**) ecs_get_id(ecs_world(),
			ecs_singleton(EcsGpuDevice));

		if (!SDL_SetGPUSwapchainParameters(device, window, selected_hdr, selected_vsync))
		{
			SDL_LogWarn(LOG_CATEGORY_CORE, "Mode not supported: %s", SDL_GetError());
		}
	}
}

[[nodiscard]]
static const char *state_string(const input_state_t state)
{
	switch (state)
	{
		case STATE_UP:
			return "Up";

		case STATE_PRESSED:
			return "Pressed";

		case STATE_DOWN:
			return "Down";

		default:
			return nullptr;
	}
}

static void draw_input_row(nk_context_t *ctx, const char *name, const input_state_t state)
{
	nk_label(ctx, name, NK_TEXT_LEFT);
	nk_label(ctx, state_string(state), NK_TEXT_LEFT);
}

static void draw_keyboard_input_row(void *userdata, const SDL_PropertiesID props, const char *name)
{
	const SDL_Keycode keycode = SDL_strtol(name, nullptr, 16);
	draw_input_row(userdata, SDL_GetKeyName(keycode), map_get(props, name, (Sint64)0));
}

static void draw_mouse_input_row(void *userdata, const SDL_PropertiesID props, const char *name)
{
	const SDL_MouseButtonFlags button = SDL_strtol(name, nullptr, 16);
	draw_input_row(userdata, mouse_button_name(button), map_get(props, name, (Sint64)0));
}

static void draw_gamepad_button_input_row(void *userdata, const SDL_PropertiesID props, const char *name)
{
	const SDL_GamepadButton button = SDL_strtol(name, nullptr, 16);
	draw_input_row(userdata, gamepad_button_name(button), map_get(props, name, (Sint64)0));
}

static void draw_gamepad_label_input_row(void *userdata, const SDL_PropertiesID props, const char *name)
{
	const SDL_GamepadButtonLabel label = SDL_strtol(name, nullptr, 16);
	draw_input_row(userdata, gamepad_button_label_name(label), map_get(props, name, (Sint64)0));
}

static void draw_name_map_row(void *userdata,
	[[maybe_unused]] const SDL_PropertiesID props, const char *name)
{
	nk_label(userdata, name, NK_TEXT_LEFT);

	// Not ideal, but it works
	const input_t input = *(const input_t*) ecs_get_id(ecs_world(),
		ecs_singleton(EcsInput));

	const input_state_t state = input_state(input, name, 0, false);
	nk_label(userdata, state_string(state), NK_TEXT_LEFT);

	const float axis = input_axis(input, name, 0);
	nk_labelf(userdata, NK_TEXT_LEFT, "%5.2f", axis);
}

void draw_debug_overlay(ecs_iter_t *iter)
{
	nk_context_t *ctx = &ecs_field(iter, nkui_context_t, 0)->nk;
	const camera_t *camera = ecs_field(iter, camera_t, 1);
	const b3BodyId player_body_id = *ecs_field(iter, b3BodyId, 2);
	SDL_Window *window = *ecs_field(iter, SDL_Window*, 3);
	const time_stats_t *time_stats = ecs_field(iter, time_stats_t, 4);
	const input_t input = *ecs_field(iter, input_t, 5);

	constexpr auto padding = 16.F;
	constexpr auto alpha = 0.75F;
	constexpr auto row_height = 18.F;

	nk_style_t *style = &ctx->style;
	nk_style_item_t *window_style = &style->window.fixed_background;

	SDL_assert(window_style->type == NK_STYLE_ITEM_COLOR);
	nk_color_t color = window_style->data.color;
	color.a = (nk_byte) ((float) color.a * alpha);

	nk_style_push_style_item(ctx, window_style, nk_style_item_color(color));

	if (nk_begin(ctx, "Debug overlay", (nk_rect_t){
		.x = padding,
		.y = padding,
		.w = 300.F,
		.h = 195.F,
	}, NK_WINDOW_BORDER))
	{
		constexpr auto ms_s = 1'000.F;

#ifndef NDEBUG
		nk_layout_row_dynamic(ctx, row_height, 1);
		nk_label(ctx, "- debug mode -", NK_TEXT_CENTERED);
		nk_label(ctx, ENGINE_NAME " " ENGINE_VERSION, NK_TEXT_LEFT);
#endif

		nk_layout_row(ctx, NK_DYNAMIC, row_height, 2, (float[]){0.3F, 0.7F});

		nk_label(ctx, "FPS", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%u (%5.2f ms)",
			time_stats->fps, iter->delta_time * ms_s);

		draw_camera_info(ctx, camera);
		draw_physics_info(ctx, player_body_id);
	}
	nk_end(ctx);

#ifdef FLECS_STATS
	if (nk_begin(ctx, "World overlay", (nk_rect_t){
		.w = 300.F,
		.h = 150.F,
		.x = padding,
		.y = (padding * 2) + 195.F,
	}, NK_WINDOW_BORDER))
	{
		const EcsWorldSummary *world_summary = ecs_get_id(ecs_world(),
			EcsWorld, ecs_id(EcsWorldSummary));

		nk_layout_row(ctx, NK_DYNAMIC, row_height, 2, (float[]){0.4F, 0.6F});

		nk_label(ctx, "Entities", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->entity_count);

		nk_label(ctx, "Tables", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->table_count);

		nk_label(ctx, "Systems", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->systems_ran_frame);

		nk_label(ctx, "Observers", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->observers_ran_frame);

		nk_label(ctx, "Queries", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->queries_ran_frame);

		nk_label(ctx, "Commands", NK_TEXT_LEFT);
		nk_labelf(ctx, NK_TEXT_LEFT, "%ld",
			world_summary->command_count_frame);
	}
	nk_end(ctx);
#endif

	if (nk_begin(ctx, "Input overlay", (nk_rect_t){
		.w = 300.F,
		.h = 310.F,
		.x = padding,
		.y = (padding * 3) + 195.F + 150.F,
	}, NK_WINDOW_BORDER))
	{
		static int selected = 0;
		static const char *sources[] = {
			"Keyboard",
			"Mouse",
			"Gamepad buttons",
			"Gamepad labels",
			"Name map",
		};

		nk_layout_row_dynamic(ctx, row_height * 1.5F, 1);
		selected = nk_combo(ctx, sources, SDL_arraysize(sources), selected,
			(int) row_height, nk_vec2(150.F, 120.F));

		nk_layout_row(ctx, NK_DYNAMIC, row_height, 2, (float[]){0.6F, 0.4F});
		switch (selected)
		{
			case 0:
				SDL_EnumerateProperties(input.key_map, draw_keyboard_input_row, ctx);
				break;

			case 1:
				SDL_EnumerateProperties(input.button_map, draw_mouse_input_row, ctx);
				break;

			case 2:
				// Maybe don't assume player 1? It's just debug though
				SDL_EnumerateProperties(input.gamepad_button_maps[0], draw_gamepad_button_input_row, ctx);
				break;

			case 3:
				// Maybe don't assume player 1? It's just debug though
				SDL_EnumerateProperties(input.gamepad_label_maps[0], draw_gamepad_label_input_row, ctx);
				break;

			case 4:
				nk_layout_row(ctx, NK_DYNAMIC, row_height, 3, (float[]){0.5F, 0.3F, 0.2F});
				SDL_EnumerateProperties(input.name_map, draw_name_map_row, ctx);
				break;

			default:
				break;
		}
	}
	nk_end(ctx);

	if (!SDL_GetWindowRelativeMouseMode(window))
	{
		int width = 0;
		SDL_GetWindowSize(window, &width, nullptr);

		constexpr float window_width = 300;
		const float window_x = (float) width - window_width - padding;

		if (nk_begin(ctx, "Debug lock info", (nk_rect_t){
				.x = window_x,
				.y = padding,
				.w = window_width,
				.h = 40.F,
			}, NK_WINDOW_BORDER
		))
		{
			nk_layout_row_dynamic(ctx, row_height, 1);
			nk_labelf(ctx, NK_TEXT_CENTERED, "Press %s to lock cursor",
				SDL_GetKeyName(SDLK_ESCAPE));
		}
		nk_end(ctx);

		if (nk_begin(ctx, "Render config", (nk_rect_t){
			.x = window_x,
			.y = (padding * 2.F) + 40.F,
			.w = window_width,
			.h = 150.F,
		}, NK_WINDOW_BORDER | NK_WINDOW_TITLE))
		{
			draw_render_config(ctx, row_height);
		}
		nk_end(ctx);
	}

	nk_style_pop_style_item(ctx);
}
