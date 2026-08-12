#include "model.h"
#include "uniformdata.h"

#include "chirp/assets.h"
#include "chirp/matrix.h"
#include "chirp/modelinfo.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>

#include <stddef.h>

typedef struct primitive_buffers
{
	SDL_GPUBuffer *vertex;
	SDL_GPUBuffer *index;
} primitive_buffers_t;

static SDL_Surface *default_texture()
{
	SDL_Surface *surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_ABGR8888);

	if (!SDL_ClearSurface(surface, 1.F, 1.F, 1.F, 1.F))
	{
		SDL_DestroySurface(surface);
		return nullptr;
	}

	return surface;
}


static bool upload_sampler(model_t *model)
{
	SDL_Surface *surface = default_texture();
	if (surface == nullptr)
	{
		return false;
	}

	const SDL_GPUSamplerCreateInfo sampler_info = {
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	};
	model->sampler = SDL_CreateGPUSampler(model->device, &sampler_info);
	if (model->sampler == nullptr)
	{
		SDL_DestroySurface(surface);
		return false;
	}

	const SDL_GPUTextureCreateInfo texture_info = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		.width = surface->w,
		.height = surface->h,
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
	};
	model->texture = SDL_CreateGPUTexture(model->device, &texture_info);
	if (model->texture == nullptr)
	{
		SDL_DestroySurface(surface);
		SDL_ReleaseGPUSampler(model->device, model->sampler);
		model->sampler = nullptr;
		return false;
	}

	// RGBA for each pixel
	const Uint32 surface_size = surface->w * surface->h * 4;

	const SDL_GPUTransferBufferCreateInfo buffer_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = surface_size,
	};
	SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(model->device, &buffer_info);
	if (transfer_buffer == nullptr)
	{
		SDL_DestroySurface(surface);
		SDL_ReleaseGPUSampler(model->device, model->sampler);
		SDL_ReleaseGPUTexture(model->device, model->texture);
		model->sampler = nullptr;
		model->texture = nullptr;
		return false;
	}

	void *transfer_data = SDL_MapGPUTransferBuffer(model->device, transfer_buffer, false);
	if (transfer_data == nullptr)
	{
		SDL_DestroySurface(surface);
		SDL_ReleaseGPUTransferBuffer(model->device, transfer_buffer);
		SDL_ReleaseGPUSampler(model->device, model->sampler);
		SDL_ReleaseGPUTexture(model->device, model->texture);
		model->sampler = nullptr;
		model->texture = nullptr;
		return false;
	}

	SDL_memcpy(transfer_data, surface->pixels, surface_size);
	SDL_UnmapGPUTransferBuffer(model->device, transfer_buffer);

	SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(model->device);
	if (command_buffer == nullptr)
	{
		SDL_DestroySurface(surface);
		SDL_ReleaseGPUTransferBuffer(model->device, transfer_buffer);
		SDL_ReleaseGPUSampler(model->device, model->sampler);
		SDL_ReleaseGPUTexture(model->device, model->texture);
		model->sampler = nullptr;
		model->texture = nullptr;
		return false;
	}

	const SDL_GPUTextureTransferInfo source = {
		.transfer_buffer = transfer_buffer,
		.offset = 0,
	};
	const SDL_GPUTextureRegion destination = {
		.texture = model->texture,
		.w = surface->w,
		.h = surface->h,
		.d = 1,
	};
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);
	SDL_UploadToGPUTexture(copy_pass, &source, &destination, false);
	SDL_EndGPUCopyPass(copy_pass);

	SDL_DestroySurface(surface);

	if (!SDL_SubmitGPUCommandBuffer(command_buffer))
	{
		SDL_ReleaseGPUTransferBuffer(model->device, transfer_buffer);
		SDL_ReleaseGPUSampler(model->device, model->sampler);
		SDL_ReleaseGPUTexture(model->device, model->texture);
		model->sampler = nullptr;
		model->texture = nullptr;
		return false;
	}

	SDL_ReleaseGPUTransferBuffer(model->device, transfer_buffer);

	return true;
}

static bool upload_mesh(SDL_GPUDevice *device, const mesh_primitive_t *primitive,
	primitive_buffers_t *buffers)
{
	const size_t vertex_size = sizeof(vertex_t) * primitive->vertex_count;
	const size_t index_size = sizeof(primitive_index_t) * primitive->index_count;

	const SDL_GPUBufferCreateInfo vertex_buffer_info = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = vertex_size,
	};
	buffers->vertex = SDL_CreateGPUBuffer(device, &vertex_buffer_info);
	if (buffers->vertex == nullptr)
	{
		return false;
	}

	const SDL_GPUBufferCreateInfo index_buffer_info = {
		.usage = SDL_GPU_BUFFERUSAGE_INDEX,
		.size = index_size,
	};
	buffers->index = SDL_CreateGPUBuffer(device, &index_buffer_info);
	if (buffers->index == nullptr)
	{
		SDL_ReleaseGPUBuffer(device, buffers->vertex);
		buffers->vertex = nullptr;
		return false;
	}

	const SDL_GPUTransferBufferCreateInfo transfer_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = vertex_size + index_size,
	};
	SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);
	if (transfer_buffer == nullptr)
	{
		SDL_ReleaseGPUBuffer(device, buffers->vertex);
		SDL_ReleaseGPUBuffer(device, buffers->index);
		buffers->vertex = nullptr;
		buffers->index = nullptr;
		return false;
	}

	void *transfer_data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
	if (transfer_data == nullptr)
	{
		SDL_ReleaseGPUBuffer(device, buffers->vertex);
		SDL_ReleaseGPUBuffer(device, buffers->index);
		SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
		buffers->vertex = nullptr;
		buffers->index = nullptr;
		return false;
	}

	SDL_memcpy(transfer_data, primitive->vertices, vertex_size);
	SDL_memcpy(transfer_data + vertex_size, primitive->indices, index_size);

	SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

	SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
	if (command_buffer == nullptr)
	{
		SDL_ReleaseGPUBuffer(device, buffers->vertex);
		SDL_ReleaseGPUBuffer(device, buffers->index);
		SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
		buffers->vertex = nullptr;
		buffers->index = nullptr;
		return false;
	}

	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);

	const SDL_GPUTransferBufferLocation vertex_source = {
		.transfer_buffer = transfer_buffer,
		.offset = 0,
	};
	const SDL_GPUBufferRegion vertex_destination = {
		.buffer = buffers->vertex,
		.offset = 0,
		.size = vertex_size,
	};
	SDL_UploadToGPUBuffer(copy_pass, &vertex_source, &vertex_destination, false);

	const SDL_GPUTransferBufferLocation index_source = {
		.transfer_buffer = transfer_buffer,
		.offset = vertex_size,
	};
	const SDL_GPUBufferRegion index_destination = {
		.buffer = buffers->index,
		.offset = 0,
		.size = index_size,
	};
	SDL_UploadToGPUBuffer(copy_pass, &index_source, &index_destination, false);

	SDL_EndGPUCopyPass(copy_pass);
	SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

	if (!SDL_SubmitGPUCommandBuffer(command_buffer))
	{
		SDL_ReleaseGPUBuffer(device, buffers->vertex);
		SDL_ReleaseGPUBuffer(device, buffers->index);
		buffers->vertex = nullptr;
		buffers->index = nullptr;
		return false;
	}

	return true;
}

static bool upload_model(model_t *model)
{
	model->buffers = SDL_calloc(model->info.node_count,
		sizeof(primitive_buffers_t*));

	for (size_t nn = 0; nn < model->info.node_count; nn++)
	{
		const model_node_t *node = model->info.nodes + nn;

		model->buffers[nn] = SDL_calloc(node->primitive_count,
			sizeof(primitive_buffers_t));

		for (size_t pp = 0; pp < node->primitive_count; pp++)
		{
			const mesh_primitive_t *primitive = node->primitives + pp;
			primitive_buffers_t *buffers = model->buffers[nn] + pp;

			if (!upload_mesh(model->device, primitive, buffers))
			{
				return false;
			}
		}
	}

	return true;
}

bool model_create(SDL_GPUDevice *device, const assets_t *assets,
	SDL_IOStream *stream, const bool close_io, model_t *model)
{
	if (!model_info_create(assets, stream, close_io, &model->info))
	{
		return false;
	}

	model->device = device;
	model->sampler = nullptr;
	model->texture = nullptr;

	if (!upload_sampler(model)
		|| !upload_model(model))
	{
		model_destroy(model);
		return false;
	}

	return true;
}

void model_destroy(model_t *model)
{
	if (model == nullptr)
	{
		return;
	}

	SDL_ReleaseGPUTexture(model->device, model->texture);
	SDL_ReleaseGPUSampler(model->device, model->sampler);

	for (size_t nn = 0; nn < model->info.node_count; nn++)
	{
		const model_node_t *node = model->info.nodes + nn;

		for (size_t pp = 0; pp < node->primitive_count; pp++)
		{
			const primitive_buffers_t *buffers = model->buffers[nn] + pp;

			SDL_ReleaseGPUBuffer(model->device, buffers->vertex);
			SDL_ReleaseGPUBuffer(model->device, buffers->index);
		}
	}

	model_info_destroy(&model->info);
}

static void mesh_draw(const model_t *model, const mesh_primitive_t *primitive, const primitive_buffers_t *buffers,
	SDL_GPURenderPass *render_pass, SDL_GPUCommandBuffer *command_buffer, const matrix4x4_t projection)
{
	const SDL_GPUBufferBinding vertex_binding = {
		.buffer = buffers->vertex,
		.offset = 0,
	};
	SDL_BindGPUVertexBuffers(render_pass, 0, &vertex_binding, 1);

	const SDL_GPUBufferBinding index_binding = {
		.buffer = buffers->index,
		.offset = 0,
	};
	SDL_BindGPUIndexBuffer(render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

	const SDL_GPUTextureSamplerBinding binding = {
		.texture = model->texture,
		.sampler = model->sampler,
	};
	SDL_BindGPUFragmentSamplers(render_pass, 0, &binding, 1);

	const vertex_uniform_data_t vertex_data = {
		.mvp = projection,
	};
	SDL_PushGPUVertexUniformData(command_buffer, 0, &vertex_data, sizeof(vertex_uniform_data_t));

	SDL_DrawGPUIndexedPrimitives(render_pass, primitive->index_count,
		1, 0, 0, 0);
}

static void node_draw(const model_t *model, const size_t node_index, SDL_GPURenderPass *render_pass,
	SDL_GPUCommandBuffer *command_buffer, const matrix4x4_t projection)
{
	const model_node_t *node = model->info.nodes + node_index;

	for (size_t i = 0; i < node->primitive_count; i++)
	{
		const mesh_primitive_t *primitive = node->primitives + i;
		const primitive_buffers_t *buffers = model->buffers[node_index] + i;

		mesh_draw(model, primitive, buffers, render_pass, command_buffer, projection);
	}
}

void model_draw(const model_t *model, SDL_GPURenderPass *render_pass,
	SDL_GPUCommandBuffer *command_buffer, const matrix4x4_t view_projection)
{
	for (size_t i = 0; i < model->info.node_count; i++)
	{
		const model_node_t *node = model->info.nodes + i;
		const matrix4x4_t projection = matrix4x4_multiply(node->world_transform, view_projection);
		node_draw(model, i, render_pass, command_buffer, projection);
	}
}

void model_draw_indexed(const model_t *model, const size_t index,
	SDL_GPURenderPass *render_pass, SDL_GPUCommandBuffer *command_buffer,
	const matrix4x4_t projection)
{
	SDL_assert(model != nullptr);
	SDL_assert(index < model->info.node_count);
	node_draw(model, index, render_pass, command_buffer, projection);
}
