#ifndef _PNGUTILS_H
#define _PNGUTILS_H

class Image // 32bpp image
{
public:    
    int   width,
          height;
    char *data;
    
    Image(int width, int height, char *data) : width(width), height(height), data(data) {}    
};

bool read_png_file(const char *filename, Image *image, bool flipVertical);

#endif // _PNGUTILS_H