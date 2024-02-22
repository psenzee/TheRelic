#include "platform/GLUtils.h"
#include "core/global.h"
#include "core/file.h"

#include <OpenGLES/ES1/glext.h>

#include <stdio.h>
#include <string.h>

typedef unsigned uint32_t;

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

unsigned ConvertEndianLEToHost(unsigned x)
{
    return x; // so convenient! :)
}

struct PVRLevel
{
    void *data;
    int   size;
};

enum ImageType { TYPE_PNG, TYPE_PVR, TYPE_UNKNOWN };

static ImageType GetImageType(const char *filename)
{
    FILE *file = fopen(globalTranslatePath(filename), "rb");
    if (!file)
        return TYPE_UNKNOWN;
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

static unsigned LoadPVRTexture(const char *filename, core::Size *size)
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
	
	file::buffer_t data = file::read_file(globalTranslatePath(filename));
	
	if (!data.data)
		return 0; // unable to load it..
	
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
			printf("V4 ");
		}
		else if (formatFlags == kPVRTextureFlagTypePVRTC_2)
		{
			internalFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			printf("V2 ");
		}
	
		maxWidth  = width  = ConvertEndianLEToHost(header.width);
		maxHeight = height = ConvertEndianLEToHost(header.height);
		
		if (size)
		   *size = core::Size(width, height);
		
		if (ConvertEndianLEToHost(header.bitmaskAlpha))
			hasAlpha = true;
		else
			hasAlpha = false;
		
		dataLength = ConvertEndianLEToHost(header.dataLength);
		
		bytes = ((uint8_t *)data.data) + sizeof(PVRTexHeader);
		
		// Calculate the data size for each texture level and respect the minimum number of blocks
		while (dataOffset < dataLength)
		{
			if (formatFlags == kPVRTextureFlagTypePVRTC_4)
			{
				blockSize = 4 * 4; // Pixel by pixel block size for 4bpp
				widthBlocks = width / 4;
				heightBlocks = height / 4;
				bpp = 4;
			}
			else
			{
				blockSize = 8 * 4; // Pixel by pixel block size for 2bpp
				widthBlocks = width / 8;
				heightBlocks = height / 4;
				bpp = 2;
			}
			
			// Clamp to minimum number of blocks
			if (widthBlocks < 2)
				widthBlocks = 2;
			if (heightBlocks < 2)
				heightBlocks = 2;

			dataSize = widthBlocks * heightBlocks * ((blockSize  * bpp) / 8);
			
			PVRLevel level;
			level.data = bytes + dataOffset;
            level.size = dataSize;
			levels[levelCount++] = level;
			
			dataOffset += dataSize;
			
			width  = MAX(width  >> 1, 1);
			height = MAX(height >> 1, 1);
		}
				  
		success = true;
	}

    if (success)
    {
	    width  = maxWidth;
	    height = maxHeight;
    
        glGenTextures(1, &_name);
    	glBindTexture(GL_TEXTURE_2D, _name);	
    	
    	for (int i = 0; i < levelCount; i++)
        {
		    PVRLevel level = levels[i];
		    glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, level.size, level.data);
		
		    int err = glGetError();
		    if (err != GL_NO_ERROR)
		    {
			    printf("Error uploading compressed texture level: %d. glError: 0x%04X", i, err);
                glDeleteTextures(1, &_name);
			    delete [] (char *)data.data;
			    return 0;
		    }
		
		    width  = MAX(width  >> 1, 1);
		    height = MAX(height >> 1, 1);
	    }
	    
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

inline unsigned NextPow2(unsigned v)
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
    unsigned uv = static_cast<unsigned>(v);
    return NextPow2(uv) == uv;
}

static bool HasAlphaChannelRGBA32(const unsigned char *data, int width, int height)
{
	enum { VALUES = 256, BYTES_PER_PIXEL = 4, ALPHA_START = 3 };
	int counts[VALUES];
	memset(counts, 0, sizeof(int) * VALUES);
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

unsigned LoadPNGTexture(const char *filename, core::Size *size)
{
    CGImageRef spriteImage;
    CGContextRef spriteContext;
    GLubyte *spriteData;
    size_t  width, height;
    
    GLuint spriteTexture = 0;

    const char *name = filename;
    
	UIImage *uiImage = [UIImage imageNamed:[NSString stringWithCString:name]];
	
    // Creates a Core Graphics image from an image file
    spriteImage = //[UIImage imageNamed:[NSString stringWithCString:name]].CGImage;
	              [uiImage CGImage];

    if (!spriteImage)
        return 0;

    // Get the width and height of the image
    width  = CGImageGetWidth(spriteImage);
    height = CGImageGetHeight(spriteImage);
    // Texture dimensions must be a power of 2. If you write an application that allows users to supply an image,
    // you'll want to add code that checks the dimensions and takes appropriate action if they are not a power of 2.        
	
    if (size)
        *size = core::Size(width, height);	

    if (!IsPow2(width) || !IsPow2(height))
    {
        printf("Bad dimensions '%s' (%ux%u)\n", name, width, height);
        [uiImage release];
        return 0;
    }
    else
    {
		printf("NG ");
//      printf("Loaded texture '%s' with dimensions %ux%u\n", name, width, height);
    }
    
    // Allocated memory needed for the bitmap context
    spriteData = (GLubyte *) malloc(width * height * 4);
    // Uses the bitmatp creation function provided by the Core Graphics framework. 
    spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    // After you create the context, you can draw the sprite image to the context.
    CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), spriteImage);
    // You don't need the context at this point, so you need to release it to avoid memory leaks.
    CGContextRelease(spriteContext);
	[uiImage release];
	spriteImage = 0;
       
    // Use OpenGL ES to generate a name for the texture.
    glGenTextures(1, &spriteTexture);
    // Bind the texture name.          
    glBindTexture(GL_TEXTURE_2D, spriteTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_LINEAR*/GL_LINEAR_MIPMAP_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);  	
	
//  Flip(spriteData, width, height, 4);

    // Specify a 2D texture image, providing the a pointer to the image data in memory	
	if (HasAlphaChannelRGBA32(spriteData, width, height))
	{
		// for now, anything with alpha is going to be 32 bits
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
	}
	else
	{
		ConvertRGBA8888ToRGB565(spriteData, width, height);
		// else 16 bits, RGB565
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, spriteData);
	}
          
    // Release the image data
    free(spriteData);

    return spriteTexture;
}

// need to flip up-down the png file before converting to PVR
// texturetool -e PVRTC --bits-per-pixel-2 -o file.pvr -f PVR file.png
static unsigned _LoadTexture(const char *filename, core::Size *size)
{
    ImageType type = GetImageType(filename);
    unsigned textureId = 0;
    switch (type)
    {
    case TYPE_PNG: textureId = LoadPNGTexture(filename, size); break;
    case TYPE_PVR: textureId = LoadPVRTexture(filename, size); break;
//	default: printf(" (unknown texture type) "); break;        
    }
//  if (!textureId)
//      printf("Failed to load texture '%s'!\n", filename);
    return textureId;
}

unsigned AttemptToLoadInSequence(const char *filename, core::Size *size, const char **types)
{
	int textureId = 0;	
	char newFilename[1024];
	while (*types)
	{
		sprintf(newFilename, *types, filename);
//		printf("Attempting to load '%s'\n", newFilename);
		textureId = _LoadTexture(newFilename, size);
		if (textureId != 0)
			return textureId;
		types++;
	}
    printf("Attempting to load '%s'\n", filename);	
	textureId = _LoadTexture(filename, size);
	if (!textureId)
		printf("\nFailed to load texture '%s' in any encoding!\n", filename);					
    return textureId;
}

extern "C" bool IsHiResDevice();

unsigned LoadTexture(const char *filename, core::Size *size)
{
	const char *hiResOrder[] = { "%s.pvrhi", "%s.pvrtc", "%s.pvrlo", 0 };
	const char *loResOrder[] = { "%s.pvrlo", "%s.pvrtc", "%s.pvrhi", 0 };
	                           //{ "%s.pvrtc", "%s.pvrlo", "%s.pvrhi", 0 };
	
    return AttemptToLoadInSequence(filename, size, IsHiResDevice() ? hiResOrder : loResOrder);
	/*
	int textureId = 0;
	char newFilename[1024];
	bool isHiRes = IsHiResDevice();
	
	sprintf(newFilename, "%s.pvrtc", filename);
	if ((textureId = _LoadTexture(newFilename, size)) == 0)
	{
		    sprintf(newFilename, isHiRes ? "%s.pvrhi" : "%s.pvrlo", filename);
            if ((textureId = _LoadTexture(newFilename, size)) == 0)
            {
                sprintf(newFilename, isHiRes ? "%s.pvrlo" : "%s.pvrhi", filename);
                if ((textureId = _LoadTexture(newFilename, size)) == 0)
                {			
                    textureId = _LoadTexture(filename, size);
                    if (!textureId)
                        printf("Failed to load texture '%s'!\n", filename);				
                }
	    }
	}
	return textureId;
	 */
}

/*
-- internalformat
Specifies the color components in the texture. Must be same as format. The following symbolic values are accepted: GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, or GL_LUMINANCE_ALPHA.
-- type
Specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, and GL_UNSIGNED_SHORT_5_5_5_1.
*/
