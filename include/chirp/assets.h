#pragma once

#include <stddef.h>

typedef struct assets_t assets_t;

[[nodiscard]]
assets_t *assets_create();

void assets_destroy(assets_t *assets);

[[nodiscard]]
void *assets_load(const assets_t *assets, const char *filename, size_t *size);
