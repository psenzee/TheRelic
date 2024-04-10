#pragma once

#include <cstdint>
#include <array>

bool parse_color(const char **s, uint32_t &color);
bool parse_color(const char **s, std::array<float, 4> &color);

struct Tuple4f;

bool parse_color(const char **s, Tuple4f &color);
