#include "read_png.h"

#include "gl_utils.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <array>
#include <iostream>
#include "pngutils.h"

static char gPVRTexIdentifier[5] = "PVR!";
static char gPNGTexIdentifier[9] = "\211PNG\r\n\032\n";

void histogram(const Image &image)
{
    // calculate histogram, this is basically just to make sure it's loading data
    typedef std::array<uint32_t, 256> channel_t;
    std::array<channel_t, 4> channels = { channel_t { 0 }, channel_t { 0 }, channel_t { 0 }, channel_t { 0 } };
    const uint8_t *p = image.data;
    for (size_t i = 0, sz = image.width * image.height * sizeof(uint32_t); i < sz; i++) {
        channel_t &channel = channels[i & 3];
        channel[*p++]++;
    }
    
    std::cout << "channels" << std::endl;
    for (const auto &channel : channels) {
        std::cout << " channel " << ((const channel_t *)&channel - (const channel_t *)&channels[0]) << std::endl;
        for (size_t i = 0; i < channel.size(); i++) {
            if (channel[i]) {
                std::cout << "  level " << i << " : " << channel[i] << std::endl;
            }
        }
    }
    std::cout << "---------" << std::endl;
}

uint32_t LoadPNGTexture(const char *filename, size2_t *size)
{
    Image image(0, 0, 0);
    if (!read_png_file(filename, &image)) {
        printf("Failed to load %s png file\n", filename);
        return 0;
    }

  	uint32_t _name = 0;
        
    printf("w:%d h:%d ", image.width, image.height);
    //histogram(image);
    		
    if (size)
        *size = size2_t { size_t(image.width), size_t(image.height) };
    
    _GL(glGenTextures(1, &_name));
    _GL(glBindTexture(GL_TEXTURE_2D, _name));
    
    //size_t size = image.width * image.height * sizeof(uint32_t);
    _GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data));
    
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    _GL(glGenerateMipmap(GL_TEXTURE_2D));
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    _GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    _GL(glBindTexture(GL_TEXTURE_2D, 0));
	
	delete [] (uint8_t *)image.data;
	return _name;
}

static void Flip(unsigned char *data, int width, int height, int bytesPerPixel)
{
    int stride = width * bytesPerPixel;
    unsigned char *scanline = new unsigned char [stride];
    for (int i = 0; i < height / 2; i++)
    {
        memcpy(scanline, data + i * stride, stride);
        memcpy(data + i * stride, data + (height - i - 1) * stride, stride);
        memcpy(data + (height - i - 1) * stride, scanline, stride);
    }
    delete [] scanline;
}

inline uint32_t NextPow2(uint32_t v)
{
    v--;
    v |= v >> 16;
    v |= v >> 8;
    v |= v >> 4;
    v |= v >> 2;
    v |= v >> 1;
    return v + 1;
}

template <typename T>
inline bool IsPow2(T v)
{
    uint32_t uv = static_cast<uint32_t>(v);
    return NextPow2(uv) == uv;
}

static bool HasAlphaChannelRGBA32(const unsigned char *data, int width, int height)
{
	enum { VALUES = 256, BYTES_PER_PIXEL = 4, ALPHA_START = 3 };
	int32_t counts[VALUES];
	memset(counts, 0, sizeof(int32_t) * VALUES);
	int stride = width * BYTES_PER_PIXEL;
	for (int i = ALPHA_START; i < stride * height; i += BYTES_PER_PIXEL)
		counts[data[i]]++;
	int countOfCounts = 0;
	for (int i = 0; i < VALUES; i++)
		if (counts[i] != 0) countOfCounts++;
	if (countOfCounts == 1)
		return false;
	return true;
}

static inline void ConvertPixelRGBA8888ToRGB565(const unsigned char *from, unsigned char *to)
{
	unsigned short pixel = ((unsigned short)(from[2] & 0xf8) >> 3) | // this mask is unnecessary, but this is not performance critical code, & I'd like consistency here then
                           ((unsigned short)(from[1] & 0xfc) << 3) |
                           ((unsigned short)(from[0] & 0xf8) << 8);
// for little endian
	to[0] = ((unsigned char *)&pixel)[0];
	to[1] = ((unsigned char *)&pixel)[1];	
// reverse for big endian, or just do a straight copy	
}

static inline void ConvertPixelRGBA8888ToRGB565(const char *from, char *to)
{
	ConvertPixelRGBA8888ToRGB565((const unsigned char *)from, (unsigned char *)to);
}

static void ConvertRGBA8888ToRGB565(unsigned char *data, int width, int height)
{
	enum { BYTES_PER_PIXEL_IN = 4, BYTES_PER_PIXEL_OUT = 2 };	
	const unsigned char *pin = data, *pend = data + BYTES_PER_PIXEL_IN * width * height;
	unsigned char *pout = data;	
	for (; pin < pend; pin += BYTES_PER_PIXEL_IN, pout += BYTES_PER_PIXEL_OUT)
		ConvertPixelRGBA8888ToRGB565(pin, pout);
}
