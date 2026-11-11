#pragma once

#include "box3d/id.h"

#include <SDL3/SDL_stdinc.h>

bool physics_create(Uint32 workers, b3WorldId *world);

void physics_destroy(b3WorldId world);

int physics_sub_step_count(Uint16 fps);
