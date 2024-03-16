#include "read_pvr.h"

#include "gl_utils.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <algorithm>

#define PVR_TEXTURE_FLAG_TYPE_MASK	0xff

static char gPVRTexIdentifier[5] = "PVR!";
static char gPNGTexIdentifier[9] = "\211PNG\r\n\032\n";

enum
{
	kPVRTextureFlagTypePVRTC_2 = 24,
	kPVRTextureFlagTypePVRTC_4
};

struct PVRTexHeader
{
	uint32_t headerLength;
	uint32_t height;
	uint32_t width;
	uint32_t numMipmaps;
	uint32_t flags;
	uint32_t dataLength;
	uint32_t bpp;
	uint32_t bitmaskRed;
	uint32_t bitmaskGreen;
	uint32_t bitmaskBlue;
	uint32_t bitmaskAlpha;
	uint32_t pvrTag;
	uint32_t numSurfs;
};

uint32_t ConvertEndianLEToHost(uint32_t x)
{
    return x; // so convenient! :)
}

struct PVRLevel
{
    void    *data;
    int32_t  size;
};

enum ImageType { TYPE_PNG, TYPE_PVR, TYPE_UNKNOWN, TYPE_NOT_FOUND };

static ImageType GetImageType(const char *filename)
{
    const char *path = filename;
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        return TYPE_NOT_FOUND;
    }
    ImageType type = TYPE_UNKNOWN;
	union Header
	{
        char          png[8];
        PVRTexHeader  pvr;
	} header;
    if (fread(&header, sizeof(header), 1, file))
    {
        if      (memcmp(&header.pvr.pvrTag, gPVRTexIdentifier, sizeof(gPVRTexIdentifier) - 1) == 0) type = TYPE_PVR;
        else if (memcmp(header.png, gPNGTexIdentifier, sizeof(gPNGTexIdentifier) - 1) == 0) type = TYPE_PNG;
    }
    fclose(file);
    return type;
}

uint32_t LoadPVRTexture(const char *filename, size2_t *size)
{
  	uint32_t _name = 0;

    enum { MAX_LEVELS = 16 }; // sixteen is plenty!

    PVRTexHeader header;
    memset(&header, 0, sizeof(header));

	bool     success = false;
	uint32_t flags = 0, pvrTag = 0;
	uint32_t dataLength = 0, dataOffset = 0, dataSize = 0;
	uint32_t blockSize = 0, widthBlocks = 0, heightBlocks = 0;
	uint32_t width = 0, height = 0, maxWidth = 0, maxHeight = 0, bpp = 4;
	uint8_t *bytes = NULL;
	uint32_t formatFlags = 0;
	bool     hasAlpha = false;
	
	uint32_t internalFormat = 0;
	
	file::buffer_t data = file::read_file(filename);
	
	if (!data.data)
		return 0; // unable to load it..
    
    printf(" data size %ul\n", data.size);
	
	header = *(PVRTexHeader *)(data.data);	
	
	pvrTag = ConvertEndianLEToHost(header.pvrTag); //?

	if (gPVRTexIdentifier[0] != ((pvrTag >>  0) & 0xff) ||
		gPVRTexIdentifier[1] != ((pvrTag >>  8) & 0xff) ||
		gPVRTexIdentifier[2] != ((pvrTag >> 16) & 0xff) ||
		gPVRTexIdentifier[3] != ((pvrTag >> 24) & 0xff))
	{
	    delete [] (char *)data.data;
		return 0;
	}
	
	flags = ConvertEndianLEToHost(header.flags);
	formatFlags = flags & PVR_TEXTURE_FLAG_TYPE_MASK;
	
	PVRLevel levels[MAX_LEVELS];
	int      levelCount = 0;
	
	if (formatFlags == kPVRTextureFlagTypePVRTC_4 || formatFlags == kPVRTextureFlagTypePVRTC_2)
	{
		if (formatFlags == kPVRTextureFlagTypePVRTC_4)
		{
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			printf("V4 fname'%s' ", filename);
		}
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
		{
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			printf("V2 fname'%s' ", filename);
		}
	
		maxWidth  = width  = ConvertEndianLEToHost(header.width);
		maxHeight = height = ConvertEndianLEToHost(header.height);
        
        printf("w:%d h:%d ", width, height);
		
		if (size)
            *size = size2_t { width, height };
		
        hasAlpha = ConvertEndianLEToHost(header.bitmaskAlpha);
		
		dataLength = ConvertEndianLEToHost(header.dataLength);
		
		bytes = ((uint8_t *)data.data) + sizeof(PVRTexHeader);
		
		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < dataLength) {
			if (formatFlags == kPVRTextureFlagTypePVRTC_4) {
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = width / 4;
				heightBlocks = height / 4;
				bpp = 4;
			} else {
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = width / 8;
				heightBlocks = height / 4;
				bpp = 2;
			}
			
			// Clamp to minimum number of blocks
            if (widthBlocks < 2) {
                widthBlocks = 2;
            }
            if (heightBlocks < 2) {
                heightBlocks = 2;
            }

			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			
			PVRLevel level;
			level.data = bytes + dataOffset;
            level.size = dataSize;
			levels[levelCount++] = level;
			
			dataOffset += dataSize;
			
            width  = std::max(unsigned(width  >> 1), unsigned(1));
			height = std::max(unsigned(height >> 1), unsigned(1));
		}
				  
		success = true;
	}

    if (success)
    {
	    width  = maxWidth;
	    height = maxHeight;
    
        _GL(glGenTextures(1, &_name));
    	_GL(glBindTexture(GL_TEXTURE_2D, _name));
    	
    	for (int i = 0; i < levelCount; i++)
        {
		    PVRLevel level = levels[i];
		    _GL(glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, level.size, level.data));
		
		    int err = glGetError();
		    if (err != GL_NO_ERROR)
		    {
			    printf("Error uploading compressed texture level: %d. glError: 0x%04X", i, err);
                _GL(glDeleteTextures(1, &_name));
			    delete [] (char *)data.data;
			    return 0;
		    }
		
		    width  = std::max(unsigned(width  >> 1), unsigned(1));
		    height = std::max(unsigned(height >> 1), unsigned(1));
	    }
	    
        _GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
        _GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        _GL(glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE));
        _GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        _GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    }
	
	delete [] (char *)data.data;
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

/*
-- internalformat
Specifies the color components in the texture. Must be same as format. The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA.
-- type
Specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1.
*/
