#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

#include "core/global.h"

#include "pngutils.h"

#include <stdio.h>
#include <string.h>

int _g_lightingType = 0;

static std::vector<int> lightingTypeStack;

enum { MAX_LIGHTING_TYPES = 6 };

void SetDefaultLightingType(int type)
{
    _g_lightingType = type % MAX_LIGHTING_TYPES;
    SetDefaultLighting();
}

void PushLightingType()
{
    lightingTypeStack.push_back(_g_lightingType);
}

void PopLightingType()
{
    if (!lightingTypeStack.empty())
    {
       SetDefaultLightingType(lightingTypeStack.back());
       lightingTypeStack.pop_back();
    }
}

int GetDefaultLightingType()
{
    return _g_lightingType;
}

void SetDefaultLighting()
{	
    glLoadIdentity();

    if (!_g_lightingType)
        GLStates::lighting.Set(false);
    else if (_g_lightingType == 1)
    {
        const GLfloat lightAmbient[]  = { 0.10f, 0.10f, 0.20f, 1.0f };
        const GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };		
        const GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 1.0f };

        //Configure OpenGL lighting

        GLStates::lighting.Set(true);
        GLStates::light[0].enable.Set(true);
        GLStates::light[1].enable.Set(true);
            
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001f);
        const GLfloat lightDiffuse1[]  = { 0.8f, 0.8f, 0.8f, 1.0f };	
        const GLfloat lightPosition1[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
        glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
    }
    else if (_g_lightingType == 2)
    {
    	const GLfloat lightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    	const GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };	
    	const GLfloat lightPosition[] = { 0.0f, 0.0f, -200.0f, 0.0f };
	
    	//Configure OpenGL lighting
    	GLStates::lighting.Set(true);    	    
    	GLStates::light[0].enable.Set(true);
        GLStates::light[1].enable.Set(false);
                	    
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001f);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glShadeModel(GL_SMOOTH);
    }
    else if (_g_lightingType == 3)
    {
        const GLfloat lightAmbient[]  = { 0.10f, 0.10f, 0.20f, 1.0f };
        const GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };		
        const GLfloat lightPosition[] = { 0.0f, 0.0f, 1.0f, 1.0f };

        //Configure OpenGL lighting

        GLStates::lighting.Set(true);
    	GLStates::light[0].enable.Set(true);
        GLStates::light[1].enable.Set(true);
            
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001f);
        const GLfloat lightDiffuse1[]  = { 0.8f, 0.8f, 0.8f, 1.0f };	
        const GLfloat lightPosition1[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
        glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
    }
    else if (_g_lightingType == 4)
    {
        GLStates::lighting.Set(true);
    	GLStates::light[0].enable.Set(true);
        GLStates::light[1].enable.Set(false);
    }
    else if (_g_lightingType == 5)
    {
        const GLfloat lightAmbient[]  = { 0.13f, 0.11f, 0.39f, 1.0f };		
        const GLfloat lightDiffuse[]  = { 0.24f, 0.18f, 0.74f, 1.0f };		
        const GLfloat lightPosition[] = { 0.0f, 0.0f, -0.5f, 1.0f };
 
        GLStates::lighting.Set(true);
    	GLStates::light[0].enable.Set(true);
        GLStates::light[1].enable.Set(false);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 			
    }
}

enum PointerType { VERTEX = 0, NORMAL, UV, MAX_POINTER_TYPES };

static void     *cachedPointers[MAX_POINTER_TYPES] = { 0 };
static unsigned  cachedBuffers[2] = { 0 };

void ClearBuffers()
{
	if (cachedBuffers[0] || cachedBuffers[1])
	{
#ifndef WIN32
	    glBindBuffer(GL_ARRAY_BUFFER,         0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
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
			GLStates::vertices.Set(true);
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
			GLStates::normals.Set(true);
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
			GLStates::uvs.Set(true);
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

#ifndef WIN32
    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER,         vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
#endif

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
    GLStates::vertices.Set(true);
    GLStates::normals.Set(true);
    GLStates::uvs.Set(true);
	
	cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

// this is essentially for Win32 only
void SetPointersInterleaved(void *data, unsigned vertices) 
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(false);
    GLStates::uvs.Set(true);

    /*
	//glInterleavedArrays(GL_T2F_N3F_V3F, 0, data);
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 32, ((char*)data + 0));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 8));
    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 32, ((char*)data + 20));
    */

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 20, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 20, ((char*)data + 12));	
    // Describe to OpenGL where the normal data is in the buffer
//  glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 20));
}

// this is essentially for Win32 only
void SetPointersInterleavedShortPos(void *data, unsigned vertices) 
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(false);
    GLStates::uvs.Set(true);

    /*
	//glInterleavedArrays(GL_T2F_N3F_V3F, 0, data);
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 32, ((char*)data + 0));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 8));
    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 32, ((char*)data + 20));
    */

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_SHORT, 16, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 16, ((char*)data + 8));	
    // Describe to OpenGL where the normal data is in the buffer
//  glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 20));
}

// this is essentially for Win32 only
void SetPointersInterleavedCharPos(void *data, unsigned vertices) 
{
	memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(false);
    GLStates::uvs.Set(true);

    /*
	//glInterleavedArrays(GL_T2F_N3F_V3F, 0, data);
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 32, ((char*)data + 0));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 8));
    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 32, ((char*)data + 20));
    */

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_BYTE,  12, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 12, ((char*)data + 4));
    // Describe to OpenGL where the normal data is in the buffer
//  glNormalPointer  (   GL_FLOAT, 32, ((char*)data + 20));
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

#ifndef WIN32
    // Activate the VBOs to draw
    glBindBuffer(GL_ARRAY_BUFFER,         vb);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
#endif

    /*
//	glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 32, (GLvoid*)((char*)NULL));	
    // Describe to OpenGL where the normal data is in the buffer
    glNormalPointer  (   GL_FLOAT, 32, (GLvoid*)((char*)NULL + 8));		
    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 32, (GLvoid*)((char*)NULL + 20));
    */

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, 20, ((char*)NULL + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, 20, ((char*)NULL + 12));	
    // Describe to OpenGL where the normal data is in the buffer
//  glNormalPointer  (   GL_FLOAT, 32, ((char*)NULL + 20));
	
    // This could actually be moved into the setup since we never disable it
    GLStates::vertices.Set(true);
    GLStates::normals.Set(false);
    GLStates::uvs.Set(true);
	
	cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

unsigned LoadTexture(const char *filename, core::Size *size)
{
	size_t	width, height;
	
	GLuint spriteTexture = 0;
	
	Image image(0, 0, 0);

    const char *name = globalTranslatePath(filename);
	
	if (read_png_file(name, &image, true))
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
  //case GL_TABLE_TOO_LARGE:   errs = "GL_TABLE_TOO_LARGE";   break;
	}
	printf("OpenGL error '%s' (%d)\n", errs, error);
}