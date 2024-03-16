#pragma once

#include "write_png.h"

#include <png.h>
#include <cstring>

bool write_png__rgba32(FILE *fp, const uint32_t *data, size_t width, size_t height)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return false;
    }

    /* Allocate/initialize the image information data.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr,  NULL);
        return false;
    }
    png_set_IHDR(png_ptr, info_ptr, png_uint_32(width), png_uint_32(height),
        8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_sRGB(png_ptr, info_ptr, PNG_sRGB_INTENT_ABSOLUTE);

    /* Set up error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    uint8_t **row_pointers = (uint8_t **)png_malloc(png_ptr, height * sizeof(uint8_t *));
    for (size_t i = 0; i < height; i++) {
        uint8_t *row = (uint8_t *)png_malloc(png_ptr, width * sizeof(uint32_t) * height);
        std::memcpy(row, data + width * i, width * sizeof(uint32_t));
        row_pointers[i] = row;
    }
    png_set_rows(png_ptr, info_ptr, row_pointers);

    /* One of the following I/O initialization functions is REQUIRED. */

    /* Set up the output control if you are using standard C streams. */
    png_init_io(png_ptr, fp);

    /* This is the easy way.  Use it if you already have all the
    * image info living in the structure.  You could "|" many
    * PNG_TRANSFORM flags into the png_transforms integer here.
    */
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    for (size_t i = 0; i < height; i++) {
        png_free(png_ptr, row_pointers[i]);
    }
    
    png_free(png_ptr, row_pointers);
    row_pointers = NULL;

    /* Clean up after the write, and free any allocated memory. */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    /* That's it! */
    return true;
}

bool write_png__rgba32(const char *filename, const uint32_t *data, size_t width, size_t height)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return false;
    }
    write_png__rgba32(fp, data, width, height);
    fclose(fp);
    return true;
}

bool write_png__grayscale16(FILE* fp, const uint16_t *data, size_t width, size_t height)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytepp row_pointers = NULL;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        return false;
    }

     if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return false;
    }

    png_set_IHDR(png_ptr, info_ptr,
                 width, height, // width and height
                 16, // bit depth
                 PNG_COLOR_TYPE_GRAY, // color type
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = (uint8_t **)png_malloc(png_ptr, height * sizeof(uint8_t *));
    for (size_t i = 0; i < height; i++) {
        uint8_t *row = (uint8_t *)png_malloc(png_ptr, width * sizeof(uint16_t) * height);
        for (size_t x = 0; x < width; x++) {
            uint16_t u = data[x + width * i];
            row[x * 2 + 0] = u >> 8;
            row[x * 2 + 1] = u & 0xff;
        }
        row_pointers[i] = row;
    }

    /* Actually write the image data. */
    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* Cleanup. */
    for (size_t i = 0; i < height; i++) {
        png_free(png_ptr, row_pointers[i]);
    }
    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    return true;
}

bool write_png__grayscale16(const char *filename, const uint16_t *data, size_t width, size_t height)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        return false;
    }
    write_png__grayscale16(fp, data, width, height);
    fclose(fp);
    return true;
}
