#pragma once

#include "chirp/assets.h"
#include "chirp/matrix.h"
#include "chirp/modelinfo.h"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>

#include <stddef.h>

typedef primitive_vertex_t vertex_t;
typedef struct primitive_buffers primitive_buffers_t;

typedef struct model
{
	SDL_GPUDevice *device;

	model_info_t info;

	primitive_buffers_t **buffers;

	SDL_GPUSampler *sampler;
	SDL_GPUTexture *texture;
} model_t;

bool model_create(SDL_GPUDevice *device, const assets_t *assets,
	SDL_IOStream *stream, bool close_io, model_t *model);

void model_destroy(model_t *model);

void model_draw(const model_t *model, SDL_GPURenderPass *render_pass,
	SDL_GPUCommandBuffer *command_buffer, matrix4x4_t view_projection);

void model_draw_indexed(const model_t *model, size_t index,
	SDL_GPURenderPass *render_pass, SDL_GPUCommandBuffer *command_buffer,
	matrix4x4_t projection);
