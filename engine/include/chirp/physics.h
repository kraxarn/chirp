#pragma once

#include "box3d/id.h"

#include <SDL3/SDL_stdinc.h>

bool physics_create(b3WorldId *world, Uint32 workers);

void physics_destroy(b3WorldId world);
