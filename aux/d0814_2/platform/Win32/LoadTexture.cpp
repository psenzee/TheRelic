#include "render/GLUtils.h"
#include "core/global.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

#include "pngutils.h"

#include <stdio.h>
#include <string.h>

unsigned LoadTexture(const char *filename, core::Size *size)
{
    size_t width, height;
    
    GLuint spriteTexture = 0;
    
    Image image(0, 0, 0);

    const char *name = globalTranslatePath(filename);
    
    if (read_png_file(name, &image, false))
    {
        // Get the width and height of the image
        width  = image.width;
        height = image.height;
        // Texture dimensions must be a power of 2. If you write an application that allows users to supply an image,
        // you'll want to add code that checks the dimensions and takes appropriate action if they are not a power of 2.        
        
        //printf("Loaded texture '%s' with dimensions %ux%u\n", filename, width, height);
        if (size)
            *size = core::Size(static_cast<int>(width), static_cast<int>(height));
        
        // Use OpenGL ES to generate a name for the texture.
        glGenTextures(1, &spriteTexture);
        // Bind the texture name.          
        glBindTexture  (GL_TEXTURE_2D, spriteTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Specify a 2D texture image, providing the a pointer to the image data in memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
        // Release the image data
        delete image.data;
    }
    else
    {
        printf("Failed to load texture '%s'!\n", name);
    }
    return spriteTexture;
}