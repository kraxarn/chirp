// C23 uses "nullptr" instead of "NULL"
#ifndef NULL
#define NULL nullptr
#endif

#include "argparse.h"
#include "tomlc17.h"
#include "cbor/common.h"
#include "cbor/data.h"
#include "cbor/maps.h"
#include "cbor/serialization.h"
#include "cbor/strings.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum
{
	MODE_NONE,
	MODE_PACK,
	MODE_UNPACK,
} operation_t;

typedef struct
{
	operation_t op;
	const char *in;
	const char *out;
} arguments_t;

bool map_add(const toml_datum_t *src, cbor_item_t *dst, const char *key)
{
	return cbor_map_add(dst, (struct cbor_pair){
		.key = cbor_move(cbor_build_string(key)),
		.value = cbor_move(cbor_build_string(toml_table_find(*src, key).u.s)),
	});
}

bool pack(const arguments_t *args)
{
	FILE *in_file = fopen(args->in, "r");
	if (in_file == nullptr)
	{
		printf("ERROR: failed to open input file");
		return false;
	}

	toml_result_t result = toml_parse_file(in_file);
	if (fclose(in_file) != 0)
	{
		printf("WARNING: failed to close input file");
	}

	if (!result.ok)
	{
		printf("ERROR: %s", result.errmsg);
		return false;
	}

	FILE *out_file = fopen(args->out, "w");
	if (out_file == nullptr)
	{
		printf("ERROR: failed to open output file");
		return false;
	}

	const toml_datum_t toml_project = toml_table_find(result.toptab, "project");

	cbor_item_t *cbor_root = cbor_new_indefinite_map();

	cbor_item_t *cbor_project = cbor_new_indefinite_map();
	map_add(&toml_project, cbor_project, "name");
	map_add(&toml_project, cbor_project, "version");
	map_add(&toml_project, cbor_project, "identifier");
	map_add(&toml_project, cbor_project, "creator");
	map_add(&toml_project, cbor_project, "copyright");
	map_add(&toml_project, cbor_project, "url");
	map_add(&toml_project, cbor_project, "type");

	cbor_map_add(cbor_root, (struct cbor_pair){
		.key = cbor_move(cbor_build_string("project")),
		.value = cbor_move(cbor_project),
	});

	const auto assets = toml_table_find(result.toptab, "assets").u.tab;

	for (auto i = 0; i < assets.size; i++)
	{
		const auto asset_files = assets.value->u.tab;
		cbor_item_t *map = cbor_new_indefinite_map();

		for (auto j = 0; j < asset_files.size; j++)
		{
			FILE *file = fopen(asset_files.value->u.s, "r");
			if (file == nullptr)
			{
				printf("WARNING: failed to open file for %s/%s", assets.key[i], asset_files.key[j]);
				continue;
			}

			fseek(file, 0, SEEK_END);
			const size_t filesize = ftell(file);
			fseek(file, 0, SEEK_SET);

			void *buffer = malloc(sizeof(unsigned char) * filesize);
			fread(buffer, sizeof(unsigned char), filesize, file);

			printf("%s > %s/%s (%lu bytes)\n", asset_files.value->u.s, assets.key[i], asset_files.key[j], filesize);

			fclose(file);
			free(buffer);
		}

		cbor_map_add(cbor_root, (struct cbor_pair){
			.key = cbor_move(cbor_build_string(assets.key[i])),
			.value = cbor_move(map),
		});
	}

	unsigned char *buffer;
	size_t buffer_size;
	cbor_serialize_alloc(cbor_root, &buffer, &buffer_size);

	if (fwrite(buffer, sizeof(unsigned char), buffer_size, out_file) != buffer_size)
	{
		printf("ERROR: failed to write output file");
		return false;
	}

	free(buffer);

	if (fclose(out_file) != 0)
	{
		printf("WARNING: failed to close output file");
	}

	toml_free(result);
	cbor_decref(&cbor_root);

	return true;
}

int main(const int argc, const char **argv)
{
	arguments_t args = {
		.op = MODE_NONE,
		.in = "",
		.out = "",
	};

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Operations"),
		OPT_BIT('p', "pack", &args.op, "pack assets", nullptr, MODE_PACK, 0),
		OPT_BIT('u', "unpack", &args.op, "unpack assets", nullptr, MODE_UNPACK, 0),
		OPT_GROUP("Options"),
		OPT_STRING('i', "input", (void*)&args.in, "file/folder to read from", nullptr, 0, 0),
		OPT_STRING('o', "output", (void*)&args.out, "file/folder to write to", nullptr, 0, 0),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, nullptr, 0);
	argparse_parse(&argparse, argc, argv);

	switch (args.op)
	{
		case MODE_PACK:
			return !pack(&args) ? 1 : 0;

		default:
			return 1;
	}
}
