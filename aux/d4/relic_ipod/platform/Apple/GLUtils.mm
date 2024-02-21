/*
 *  GLUtils.mm
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/26/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "GLUtils.h"
#include "core/global.h"
#include <OpenGLES/ES1/glext.h>

#include <stdio.h>
#include <string.h>

enum PointerType { VERTEX = 0, NORMAL, UV, MAX_POINTER_TYPES };

static void     *cachedPointers[MAX_POINTER_TYPES] = { 0 };
static unsigned  cachedBuffers[2] = { 0 };

void SetDefaultLightingType(int type)
{
}

void ClearBuffers()
{
	if (cachedBuffers[0] || cachedBuffers[1])
	{
	    glBindBuffer(GL_ARRAY_BUFFER,         0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		cachedBuffers[0] = cachedBuffers[1] = 0;
	}
}

void ClearCachedPointers()
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));
	ClearBuffers();
}

void SetVertexPointer(void *data)
{
	if (cachedPointers[VERTEX] != data)
	{
		ClearBuffers();		
		if (data)
			glEnableClientState(GL_VERTEX_ARRAY);	
	    glVertexPointer(3, GL_FLOAT, 0, data);
		cachedPointers[VERTEX] = data;
	}
}

void SetNormalPointer(void *data)
{
	if (cachedPointers[NORMAL] != data)
	{
		ClearBuffers();		
		if (data)
			glEnableClientState(GL_NORMAL_ARRAY);
	    glNormalPointer(GL_FLOAT, 0, data);	
	    cachedPointers[NORMAL] = data;
    }
}

void SetUvPointer(void *data)
{
	if (cachedPointers[UV] != data)
	{
		ClearBuffers();		
		if (data)
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    	glTexCoordPointer(2, GL_FLOAT, 0, data);		
	    cachedPointers[UV] = data;
    }
}

void SetBuffers(unsigned vb, unsigned ib, unsigned vertices)
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));	
	
	if (!vb)
	{
		ClearBuffers();
		return;
	}
	
	if (vb == cachedBuffers[0] && ib == cachedBuffers[1])
		return;
	
    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER,         vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

    const GLsizeiptr vertexSize = vertices * 3 * sizeof(float);
    const GLsizeiptr uvSize     = vertices * 2 * sizeof(float);
    const GLsizeiptr normalSize = vertices * 3 * sizeof(float);	  

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer(3, GL_FLOAT, 0, (GLvoid*)((char*)NULL));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 0, (GLvoid*)((char*)NULL + vertexSize));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer(GL_FLOAT, 0, (GLvoid*)((char*)NULL + vertexSize + uvSize));

    // This could actually be moved into the setup since we never disable it
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

void SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices)
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));	
	
	if (!vb)
	{
		ClearBuffers();
		return;
	}
	
	if (vb == cachedBuffers[0] && ib == cachedBuffers[1])
		return;
	
    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER,         vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	
//	glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 32, (GLvoid*)((char*)NULL));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer  (   GL_FLOAT, 32, (GLvoid*)((char*)NULL + 8));		
    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 32, (GLvoid*)((char*)NULL + 20));
	
    // This could actually be moved into the setup since we never disable it
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

GLuint LoadTexture(const char *filename, core::Size *size)
{
	CGImageRef spriteImage;
	CGContextRef spriteContext;
	GLubyte *spriteData;
	size_t	width, height;
	
	GLuint spriteTexture = 0;
	
//	const char *name = globalTranslatePath(filename);
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
		
		printf("Loaded texture '%s' with dimensions %ux%u\n", name, width, height);
		if (size)
			*size = core::Size(width, height);
		
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
		/*
		// Set the texture parameters to use a minifying filter and a linear filer (weighted average)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		// Enable use of the texture
		glEnable(GL_TEXTURE_2D);
		// Set a blending function to use
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		// Enable blending
		glEnable(GL_BLEND);
		 */
	}
	else
	{
        printf("Failed to load texture '%s'!\n", name);
	}
	return spriteTexture;
}

void Print(const char *label, const Vector3 &v)
{
	printf("%s (%.2f, %.2f, %.2f)\n", label, v.x, v.y, v.z);
}

void Print(const Vector3 &v)
{
	Print("v", v);
}

void Print(const char *label, const Matrix &v)
{
	printf("matrix %s\n", label);
	printf("| %-8.2f %-8.2f %-8.2f %-8.2f |\n", v.data[0],  v.data[1],  v.data[2],  v.data[3]);
	printf("| %-8.2f %-8.2f %-8.2f %-8.2f |\n", v.data[4],  v.data[5],  v.data[6],  v.data[7]);	
	printf("| %-8.2f %-8.2f %-8.2f %-8.2f |\n", v.data[8],  v.data[9],  v.data[10], v.data[11]);		
	printf("| %-8.2f %-8.2f %-8.2f %-8.2f |\n", v.data[12], v.data[13], v.data[14], v.data[15]);
}

void PrintGLError()
{
	int error = glGetError();
	if (error == GL_NO_ERROR)
		return;
	const char *errs = "<unknown>";
	switch (error)
	{
	case GL_INVALID_ENUM:      errs = "GL_INVALID_ENUM";      break;
	case GL_INVALID_OPERATION: errs = "GL_INVALID_OPERATION"; break;
	case GL_INVALID_VALUE:     errs = "GL_INVALID_VALUE";     break;
	case GL_OUT_OF_MEMORY:     errs = "GL_OUT_OF_MEMORY";     break;
	case GL_STACK_OVERFLOW:    errs = "GL_STACK_OVERFLOW";    break;
	case GL_STACK_UNDERFLOW:   errs = "GL_STACK_UNDERFLOW";   break;			
	//case GL_TABLE_TOO_LARGE: errs = "GL_TABLE_TOO_LARGE";   break;			
	}
	printf("OpenGL error '%s' (%d)\n", errs, error);
}