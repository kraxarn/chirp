#pragma once

#include "chirp/assets.h"
#include "chirp/matrix.h"
#include "chirp/vector.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_stdinc.h>

#include <stddef.h>

typedef Uint16 primitive_index_t;

typedef struct material material_t;
typedef struct scene_camera scene_camera_t;

typedef struct primitive_vertex
{
	vector3f_t position;
	vector3f_t normal;
	vector2f_t tex_coord;
	vector4f_t color;
} primitive_vertex_t;

typedef struct mesh_primitive
{
	primitive_vertex_t *vertices;
	size_t vertex_count;

	primitive_index_t *indices;
	size_t index_count;
} mesh_primitive_t;

typedef struct model_node
{
	char *name;

	mesh_primitive_t *primitives;
	size_t primitive_count;

	const matrix4x4_t world_transform;
	vector3f_t translation;
} model_node_t;

typedef struct model_info
{
	material_t *materials;
	size_t material_count;

	model_node_t *nodes;
	size_t node_count;

	scene_camera_t *cameras;
	size_t camera_count;
} model_info_t;

bool model_info_create(const assets_t *assets, SDL_IOStream *stream,
	bool close_io, model_info_t *model);

void model_info_destroy(model_info_t *model);

[[nodiscard]]
const char *model_node_name(const model_info_t *model, size_t index);

[[nodiscard]]
matrix4x4_t model_node_world_transform(const model_info_t *model, size_t index);

[[nodiscard]]
vector3f_t model_node_translation(const model_info_t *model, size_t index);
