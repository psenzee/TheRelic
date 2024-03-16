#pragma once

#include <array>
#include <cstddef>

typedef std::array<size_t, 2> size2_t;

uint32_t LoadPNGTexture(const char *filename, size2_t *size);
