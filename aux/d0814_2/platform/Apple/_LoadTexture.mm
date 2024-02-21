#include "platform/GLUtils.h"
#include "core/global.h"

#include <OpenGLES/ES1/glext.h>

#include <stdio.h>
#include <string.h>

// $TODO this is an inefficient way to do this
template <typename T>
bool IsPow2(T x)
{
	unsigned u = (unsigned)x;
	for (int i = 0; (1u << i) <= u; i++)
		if ((1u << i) == u)
			return true;
	return false;
}

GLuint LoadTexture(const char *filename, core::Size *size)
{
    CGImageRef spriteImage;
    CGContextRef spriteContext;
    GLubyte *spriteData;
    size_t  width, height;
    
    GLuint spriteTexture = 0;
    
//  const char *name = globalTranslatePath(filename);
    const char *name = filename;
    
    // Creates a Core Graphics image from an image file
    spriteImage = [UIImage imageNamed:[NSString stringWithCString:name]].CGImage;
    
    if (spriteImage)
    {
        // Get the width and height of the image
        width  = CGImageGetWidth(spriteImage);
        height = CGImageGetHeight(spriteImage);
        // Texture dimensions must be a power of 2. If you write an application that allows users to supply an image,
        // you'll want to add code that checks the dimensions and takes appropriate action if they are not a power of 2.        
        
        if (size)
            *size = core::Size(width, height);
		if (!IsPow2(width) || !IsPow2(height)) 
		{
            printf("Bad texture dimensions for texture '%s' (%ux%u)\n", name, width, height);						
		}
		else
	    {
            printf("Loaded texture '%s' with dimensions %ux%u\n", name, width, height);
		}
        
        // Allocated memory needed for the bitmap context
        spriteData = (GLubyte *) malloc(width * height * 4);
        // Uses the bitmatp creation function provided by the Core Graphics framework. 
        spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
        // After you create the context, you can draw the sprite image to the context.
        CGContextDrawImage(spriteContext, CGRectMake(0.0, 0.0, (CGFloat)width, (CGFloat)height), spriteImage);
        // You don't need the context at this point, so you need to release it to avoid memory leaks.
        CGContextRelease(spriteContext);
        
        // Use OpenGL ES to generate a name for the texture.
        glGenTextures(1, &spriteTexture);
        // Bind the texture name.          
        glBindTexture(GL_TEXTURE_2D, spriteTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR/*GL_LINEAR_MIPMAP_NEAREST*/);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR/*GL_LINEAR*/);      
        glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);            
        // Specify a 2D texture image, providing the a pointer to the image data in memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
        // Release the image data
        free(spriteData);
    }
    else
    {
        printf("Failed to load texture '%s'!\n", name);
    }
    return spriteTexture;
}