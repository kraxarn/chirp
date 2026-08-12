#include "chirp/modelinfo.h"
#include "chirp/assets.h"
#include "chirp/logcategory.h"
#include "chirp/matrix.h"
#include "chirp/vector.h"

#include "cgltf.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>

#include <stddef.h>

#define model_property_t      cgltf_attribute_type
#define prop_vertex_position  cgltf_attribute_type_position
#define prop_vertex_normal    cgltf_attribute_type_normal
#define prop_vertex_tex_coord cgltf_attribute_type_texcoord
#define prop_index            cgltf_attribute_type_custom

typedef struct material
{
	const char *name;
	SDL_FColor *color;
} material_t;

typedef struct scene_camera
{
	char *name;
} scene_camera_t;

static void *gltf_alloc([[maybe_unused]] void *user,
	const cgltf_size size)
{
	return SDL_malloc(size);
}

static void gltf_free([[maybe_unused]] void *user, void *ptr)
{
	SDL_free(ptr);
}

static cgltf_result gltf_read([[maybe_unused]] const cgltf_memory_options *memory_options,
	const cgltf_file_options *file_options, const char *path, cgltf_size *size, void **data)
{
	const char *ext = SDL_strrchr(path, '.');
	if (ext == nullptr)
	{
		return cgltf_result_unknown_format;
	}

	const bool is_buffer = SDL_strcmp(ext, ".bin") == 0;
	char *asset_name = nullptr;
	SDL_asprintf(&asset_name, "models/%s/%.*s",
		(int) is_buffer ? "buffers" : "images",
		(int) (ext - path), path
	);

	const assets_t *assets = file_options->user_data;
	SDL_IOStream *stream = assets_load(assets, asset_name);
	if (stream == nullptr)
	{
		return cgltf_result_file_not_found;
	}

	*data = SDL_LoadFile_IO(stream, size, true);
	return cgltf_result_success;
}

[[nodiscard]]
static const char *cgltf_error_string(const cgltf_result result)
{
	switch (result)
	{
		case cgltf_result_data_too_short: return "Data too short";
		case cgltf_result_unknown_format: return "Unknown format";
		case cgltf_result_invalid_json: return "Invalid JSON";
		case cgltf_result_invalid_gltf: return "Invalid glTF";
		case cgltf_result_invalid_options: return "Invalid options";
		case cgltf_result_file_not_found: return "File not found";
		case cgltf_result_io_error: return "I/O error";
		case cgltf_result_out_of_memory: return "Out of memory";
		case cgltf_result_legacy_gltf: return "Legacy glTF";
		default: return "Unknown error";
	}
}

[[nodiscard]]
static const char *cgltf_primitive_type_string(const cgltf_primitive_type type)
{
	switch (type)
	{
		case cgltf_primitive_type_invalid: return "invalid";
		case cgltf_primitive_type_points: return "points";
		case cgltf_primitive_type_lines: return "lines";
		case cgltf_primitive_type_line_loop: return "line loop";
		case cgltf_primitive_type_line_strip: return "line strip";
		case cgltf_primitive_type_triangles: return "triangles";
		case cgltf_primitive_type_triangle_strip: return "triangle strip";
		case cgltf_primitive_type_triangle_fan: return "triangle fan";
		default: return "unknown";
	}
}

[[nodiscard]]
static const char *cgltf_type_string(const cgltf_type type)
{
	switch (type)
	{
		case cgltf_type_invalid: return "invalid";
		case cgltf_type_scalar: return "scalar";
		case cgltf_type_vec2: return "vector2";
		case cgltf_type_vec3: return "vector3";
		case cgltf_type_vec4: return "vector4";
		case cgltf_type_mat2: return "matrix2x2";
		case cgltf_type_mat3: return "matrix3x3";
		case cgltf_type_mat4: return "matrix4x4";
		default: return "unknown";
	}
}

[[nodiscard]]
static const char *cgltf_component_type_string(const cgltf_component_type type)
{
	switch (type)
	{
		case cgltf_component_type_invalid: return "invalid";
		case cgltf_component_type_r_8: return "byte";
		case cgltf_component_type_r_8u: return "unsigned byte";
		case cgltf_component_type_r_16: return "short";
		case cgltf_component_type_r_16u: return "unsigned short";
		case cgltf_component_type_r_32u: return "unsigned int";
		case cgltf_component_type_r_32f: return "float";
		default: return "unknown";
	}
}

[[nodiscard]]
static const char *cgltf_attribute_type_string(const cgltf_attribute_type type)
{
	switch (type)
	{
		case cgltf_attribute_type_invalid: return "invalid";
		case cgltf_attribute_type_position: return "position";
		case cgltf_attribute_type_normal: return "normal";
		case cgltf_attribute_type_tangent: return "tangent";
		case cgltf_attribute_type_texcoord: return "tex coord";
		case cgltf_attribute_type_color: return "color";
		case cgltf_attribute_type_joints: return "joints";
		case cgltf_attribute_type_weights: return "weights";
		case cgltf_attribute_type_custom: return "custom";
		default: return "unknown";
	}
}

static size_t append_debug_info(char *str, const size_t str_len,
	const char *key, const cgltf_size value)
{
	if (value <= 0)
	{
		return 0;
	}

	constexpr size_t temp_len = 32;
	char temp[temp_len];
	if (SDL_snprintf(temp, temp_len, ", %s: %zu", key, value) < 0)
	{
		return 0;
	}

	return SDL_strlcat(str, temp, str_len);
}

static void log_debug_info(const cgltf_data *data)
{
	constexpr size_t type_str_len = 5;
	char type_str[type_str_len];

	switch (data->file_type)
	{
		case cgltf_file_type_gltf:
			SDL_strlcpy(type_str, "gltf", type_str_len);
			break;

		case cgltf_file_type_glb:
			SDL_strlcpy(type_str, "glb", type_str_len);
			break;

		default:
			SDL_LogWarn(LOG_CATEGORY_MODEL, "Unknown model format");
			return;
	}

	constexpr size_t model_info_len = 256;
	char model_info[model_info_len];
	SDL_zeroa(model_info);

	cgltf_size mesh_primitive_count = 0;
	for (size_t i = 0; i < data->meshes_count; i++)
	{
		mesh_primitive_count += data->meshes[i].primitives_count;
	}

	append_debug_info(model_info, model_info_len, "meshes", data->meshes_count);
	append_debug_info(model_info, model_info_len, "primitives", mesh_primitive_count);
	append_debug_info(model_info, model_info_len, "materials", data->materials_count);
	append_debug_info(model_info, model_info_len, "accessors", data->accessors_count);
	append_debug_info(model_info, model_info_len, "buffer views", data->buffer_views_count);
	append_debug_info(model_info, model_info_len, "buffers", data->buffers_count);
	append_debug_info(model_info, model_info_len, "images", data->images_count);
	append_debug_info(model_info, model_info_len, "textures", data->textures_count);
	append_debug_info(model_info, model_info_len, "samplers", data->samplers_count);
	append_debug_info(model_info, model_info_len, "skins", data->skins_count);
	append_debug_info(model_info, model_info_len, "cameras", data->cameras_count);
	append_debug_info(model_info, model_info_len, "lights", data->lights_count);
	append_debug_info(model_info, model_info_len, "nodes", data->nodes_count);
	append_debug_info(model_info, model_info_len, "scenes", data->scenes_count);
	append_debug_info(model_info, model_info_len, "animations", data->animations_count);
	append_debug_info(model_info, model_info_len, "variants", data->variants_count);
	append_debug_info(model_info, model_info_len, "extensions", data->data_extensions_count);
	append_debug_info(model_info, model_info_len, "extensions used", data->extensions_used_count);
	append_debug_info(model_info, model_info_len, "extensions required", data->extensions_required_count);

	SDL_LogDebug(LOG_CATEGORY_MODEL, "Loaded model (type: %s%s)", type_str, model_info);
}

static void print_materials(const material_t *materials, const size_t count)
{
	for (size_t i = 0; i < count; i++)
	{
		const material_t *material = materials + i;

		const SDL_FColor *color = material->color;

		SDL_LogDebug(LOG_CATEGORY_MODEL, "Material: %s, #%02x%02x%02x%02x", material->name,
			(Uint8) (color->r * SDL_ALPHA_OPAQUE),
			(Uint8) (color->g * SDL_ALPHA_OPAQUE),
			(Uint8) (color->b * SDL_ALPHA_OPAQUE),
			(Uint8) (color->a * SDL_ALPHA_OPAQUE)
		);
	}
}

static bool load_materials(model_info_t *model, const cgltf_data *gltf_data)
{
	model->material_count = gltf_data->materials_count;
	model->materials = SDL_malloc(sizeof(material_t) * model->material_count);

	if (model->materials == nullptr)
	{
		return false;
	}

	for (cgltf_size i = 0; i < gltf_data->materials_count; i++)
	{
		const cgltf_material *material = gltf_data->materials + i;

		model->materials[i].name = material->name;
		model->materials[i].color = (SDL_FColor*) material->pbr_metallic_roughness.base_color_factor;
	}

	print_materials(model->materials, model->material_count);
	return true;
}

static bool load_buffer_data(const cgltf_accessor *accessor, mesh_primitive_t *primitive,
	const model_property_t property)
{
	cgltf_type expected_type;
	cgltf_component_type expected_component_type;

	switch (property)
	{
		case prop_index:
			expected_type = cgltf_type_scalar;
			expected_component_type = cgltf_component_type_r_16u;
			break;

		case prop_vertex_position:
		case prop_vertex_normal:
			expected_type = cgltf_type_vec3;
			expected_component_type = cgltf_component_type_r_32f;
			break;

		case prop_vertex_tex_coord:
			expected_type = cgltf_type_vec2;
			expected_component_type = cgltf_component_type_r_32f;
			break;

		default:
			return SDL_SetError("Invalid property: %d", property);
	}

	if (accessor->type != expected_type
		|| accessor->component_type != expected_component_type)
	{
		return SDL_SetError("Invalid accessor type: %s %s, expected %s %s",
			cgltf_type_string(accessor->type),
			cgltf_component_type_string(accessor->component_type),
			cgltf_type_string(expected_type),
			cgltf_component_type_string(expected_component_type)
		);
	}

	if (property == prop_index
		&& primitive->indices == nullptr)
	{
		primitive->index_count = accessor->count;
		primitive->indices = SDL_calloc(primitive->index_count,
			sizeof(primitive_index_t));
	}

	if (property != prop_index
		&& primitive->vertices == nullptr)
	{
		primitive->vertex_count = accessor->count;
		primitive->vertices = SDL_calloc(primitive->vertex_count,
			sizeof(primitive_vertex_t));
	}

	if (property == prop_index)
	{
		const cgltf_size count = cgltf_accessor_unpack_indices(accessor, primitive->indices,
			sizeof(primitive_index_t), accessor->count);

		if (count != primitive->index_count)
		{
			return SDL_SetError("Invalid index count, found %zu but expected %zu",
				count, primitive->index_count);
		}

		return true;
	}

	const cgltf_size num_components = cgltf_num_components(accessor->type);
	const cgltf_size float_count = accessor->count * num_components;

	cgltf_float *out = SDL_calloc(float_count, sizeof(cgltf_float));
	if (out == nullptr)
	{
		return SDL_SetError("Failed to allocate memory");
	}

	const cgltf_size count = cgltf_accessor_unpack_floats(accessor, out, float_count);
	if (count != primitive->vertex_count * num_components)
	{
		SDL_free(out);

		return SDL_SetError("Invalid %s count, found %zu but expected %zu",
			cgltf_attribute_type_string(property),
			count / num_components, primitive->vertex_count
		);
	}

	for (size_t i = 0; i < accessor->count; i++)
	{
		primitive_vertex_t *vertex = primitive->vertices + i;
		const cgltf_float *data = out + (i * num_components);

		if (property == prop_vertex_tex_coord)
		{
			vertex->tex_coord.x = data[0];
			vertex->tex_coord.y = data[1];
			continue;
		}

		vector3f_t *target = property == prop_vertex_position
			? &vertex->position
			: &vertex->normal;

		target->x = data[0];
		target->y = data[1];
		target->z = data[2];
	}

	SDL_free(out);
	return true;
}

[[nodiscard]]
static bool supported_attribute(const cgltf_attribute_type type)
{
	return (bool) (type == prop_vertex_position
		|| type == prop_vertex_normal
		|| type == prop_vertex_tex_coord
		|| type == prop_index);
}

static void set_primitive_material(const mesh_primitive_t *primitive,
	const material_t *material)
{
	for (size_t j = 0; j < primitive->vertex_count; j++)
	{
		primitive_vertex_t *vertex = primitive->vertices + j;
		vertex->color = *((vector4f_t*) material->color);
	}
}

static bool load_model_data(model_info_t *model, const cgltf_data *gltf_data)
{
	model->node_count = gltf_data->nodes_count;
	model->nodes = SDL_calloc(sizeof(model_node_t), model->node_count);

	for (size_t nn = 0; nn < gltf_data->nodes_count; nn++)
	{
		const cgltf_node *gltf_node = gltf_data->nodes + nn;
		model_node_t *node = model->nodes + nn;

		if (gltf_node->name == nullptr)
		{
			SDL_LogWarn(LOG_CATEGORY_MODEL, "Node %zu does not have a name", nn + 1);
			SDL_asprintf(&node->name, "node%02zu", nn);
		}
		else
		{
			SDL_LogDebug(LOG_CATEGORY_MODEL, "Found node: %s", gltf_node->name);
			node->name = SDL_strdup(gltf_node->name);
		}

		node->translation = *((vector3f_t*) gltf_node->translation);

		const cgltf_mesh *gltf_mesh = gltf_node->mesh;
		if (gltf_mesh == nullptr)
		{
			continue;
		}

		cgltf_node_transform_world(gltf_node, (cgltf_float*) &node->world_transform.m);

		node->primitive_count = gltf_mesh->primitives_count;
		node->primitives = SDL_calloc(node->primitive_count, sizeof(mesh_primitive_t));

		for (size_t pp = 0; pp < gltf_mesh->primitives_count; pp++)
		{
			const cgltf_primitive *gltf_primitive = gltf_mesh->primitives + pp;

			if (gltf_primitive->type != cgltf_primitive_type_triangles)
			{
				return SDL_SetError("Invalid primitive: %s",
					cgltf_primitive_type_string(gltf_primitive->type));
			}

			if (gltf_primitive->has_draco_mesh_compression)
			{
				return SDL_SetError("Draco compression is not supported");
			}

			mesh_primitive_t *primitive = node->primitives + pp;

			primitive->vertices = nullptr;
			primitive->vertex_count = 0;

			primitive->indices = nullptr;
			primitive->index_count = 0;

			if (gltf_primitive->indices != nullptr
				&& !load_buffer_data(gltf_primitive->indices, primitive, prop_index))
			{
				return false;
			}

			for (cgltf_size aa = 0; aa < gltf_primitive->attributes_count; aa++)
			{
				const cgltf_attribute *gltf_attribute = gltf_primitive->attributes + aa;

				if (!supported_attribute(gltf_attribute->type)
					|| !load_buffer_data(gltf_attribute->data, primitive, gltf_attribute->type))
				{
					if (SDL_strlen(SDL_GetError()) > 0)
					{
						return false;
					}

					return SDL_SetError("Unsupported attribute: %s (%s %s)",
						cgltf_attribute_type_string(gltf_attribute->type),
						cgltf_type_string(gltf_attribute->data->type),
						cgltf_component_type_string(gltf_attribute->data->component_type)
					);
				}
			}

			for (size_t mm = 0; mm < model->material_count; mm++)
			{
				const material_t *material = model->materials + mm;
				if (SDL_strcmp(gltf_primitive->material->name, material->name) == 0)
				{
					set_primitive_material(primitive, material);
					break;
				}
			}
		}
	}

	return true;
}

static bool load_cameras(model_info_t *model, const cgltf_data *gltf_data)
{
	model->camera_count = gltf_data->cameras_count;
	if (model->camera_count == 0)
	{
		return true;
	}

	model->cameras = SDL_calloc(model->camera_count, sizeof(scene_camera_t));
	if (model->cameras == nullptr)
	{
		return false;
	}

	for (size_t cc = 0; cc < model->camera_count; cc++)
	{
		const cgltf_camera *gltf_camera = gltf_data->cameras + cc;
		SDL_LogDebug(LOG_CATEGORY_MODEL, "Found camera: %s", gltf_camera->name);

		scene_camera_t *camera = model->cameras + cc;
		camera->name = SDL_strdup(gltf_camera->name);
	}

	return true;
}

bool model_info_create(const assets_t *assets, SDL_IOStream *stream,
	const bool close_io, model_info_t *model)
{
	size_t file_size = 0;
	void *file_data = SDL_LoadFile_IO(stream, &file_size, close_io);
	if (file_data == nullptr)
	{
		return false;
	}

	model->materials = nullptr;
	model->material_count = 0;

	model->cameras = nullptr;
	model->camera_count = 0;

	const Uint64 begin = SDL_GetTicks();

	const cgltf_options options = {
		.type = cgltf_file_type_gltf,
		.memory = (cgltf_memory_options){
			.alloc_func = gltf_alloc,
			.free_func = gltf_free,
		},
		.file = (cgltf_file_options){
			.read = gltf_read,
			.user_data = (void*) assets,
		}
	};

	cgltf_data *gltf_data = nullptr;

	cgltf_result result = cgltf_parse(&options, file_data, file_size, &gltf_data);
	if (result != cgltf_result_success)
	{
		SDL_SetError("%s", cgltf_error_string(result));
		cgltf_free(gltf_data);
		SDL_free(file_data);
		return false;
	}

	const Uint64 parse_end = SDL_GetTicks();
	SDL_LogDebug(LOG_CATEGORY_MODEL, "Parsed model in %lu ms", parse_end - begin);

	result = cgltf_load_buffers(&options, gltf_data, ".");
	if (result != cgltf_result_success)
	{
		SDL_SetError("%s", cgltf_error_string(result));
		cgltf_free(gltf_data);
		SDL_free(file_data);
		return false;
	}

	const Uint64 buffer_end = SDL_GetTicks();
	SDL_LogDebug(LOG_CATEGORY_MODEL, "Loaded buffers in %lu ms", buffer_end - parse_end);

	log_debug_info(gltf_data);

	if (!load_materials(model, gltf_data)
		|| !load_model_data(model, gltf_data)
		|| !load_cameras(model, gltf_data))
	{
		cgltf_free(gltf_data);
		SDL_free(file_data);
		return false;
	}

	cgltf_free(gltf_data);
	SDL_free(file_data);

	const Uint64 model_end = SDL_GetTicks();
	SDL_LogDebug(LOG_CATEGORY_MODEL, "Loaded model data in %lu ms", model_end - buffer_end);

	return true;
}

void model_info_destroy(model_info_t *model)
{
	if (model == nullptr)
	{
		return;
	}

	SDL_free(model->materials);

	for (size_t cc = 0; cc < model->camera_count; cc++)
	{
		const scene_camera_t *camera = model->cameras + cc;
		SDL_free(camera->name);
	}
	SDL_free(model->cameras);

	for (size_t nn = 0; nn < model->node_count; nn++)
	{
		const model_node_t *node = model->nodes + nn;

		SDL_free(node->name);

		for (size_t pp = 0; pp < node->primitive_count; pp++)
		{
			const mesh_primitive_t *primitive = node->primitives + pp;

			SDL_free(primitive->vertices);
			SDL_free(primitive->indices);
		}
		SDL_free(node->primitives);
	}
	SDL_free(model->nodes);
}

const char *model_node_name(const model_info_t *model, const size_t index)
{
	SDL_assert(model != nullptr);
	SDL_assert(index < model->node_count);
	return model->nodes[index].name;
}

matrix4x4_t model_node_world_transform(const model_info_t *model, const size_t index)
{
	SDL_assert(model != nullptr);
	SDL_assert(index < model->node_count);
	return model->nodes[index].world_transform;
}

vector3f_t model_node_translation(const model_info_t *model, const size_t index)
{
	SDL_assert(model != nullptr);
	SDL_assert(index < model->node_count);
	return model->nodes[index].translation;
}
