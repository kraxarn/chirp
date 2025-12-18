#pragma once

void script_engine_create();

void script_engine_destroy();

bool script_engine_exec(const char *source, const char *filename);
