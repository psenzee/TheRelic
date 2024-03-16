#pragma once

#include <cstdlib>
#include <cstdio>
#include <cstddef>

bool write_png__rgba32(FILE *fp, const uint32_t *data, size_t width, size_t height);
bool write_png__rgba32(const char *filename, const uint32_t *data, size_t width, size_t height);
bool write_png__grayscale16(FILE* fp, const uint16_t *data, size_t width, size_t height);
bool write_png__grayscale16(const char *filename, const uint16_t *data, size_t width, size_t height);
