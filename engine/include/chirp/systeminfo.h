#pragma once

[[nodiscard]]
const char *system_info_cpu_name();

[[nodiscard]]
const char *system_info_arch_name();

[[nodiscard]]
bool system_info_cpu_supported();
