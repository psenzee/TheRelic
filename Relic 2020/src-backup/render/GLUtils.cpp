#include "GLUtils.h"
#include "core/global.h"
#include "platform/GLIncludes.h"
#include "render/GLStates.h"

#include <stdio.h>
#include <string.h>
#include <vector>

int _g_lightingType = 0;

static std::vector<int> lightingTypeStack;

enum { MAX_LIGHTING_TYPES = 6 };

void SetDefaultLightingType(int type)
{
    _g_lightingType = type % MAX_LIGHTING_TYPES;
    SetDefaultLighting();
}

void SetActiveTexture(int value)
{
#ifdef WIN32
    static PFNGLACTIVETEXTUREPROC glActiveTexture = 0;
    if (!glActiveTexture)
        glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
#endif
    glActiveTexture(value);
}

void SetAsTexture0(int id)
{
   SetActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, id);
 //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // note that this can also be GL_REPLACE or GL_DECAL, however, they will kill any lighting
}

void SetAsTexture1(int id)
{
   SetActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, id);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
}

void SetColorCombineMode(int combine) // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE
{
	/*
   glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,  combine);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,  GL_PREVIOUS);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,  GL_TEXTURE);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	 */
}

void SetAlphaCombineMode(int combine) // combine GL_ADD/GL_MODULATE/GL_INTERPOLATE
{
	/*
   glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,  combine);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,  GL_PREVIOUS);
   glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA,  GL_TEXTURE);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
   glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
	 */
}

void ClearAll()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ClearZBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
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
        const GLfloat lightAmbient[]  = { 0.80f, 0.80f, 0.90f, 1.0f };
        const GLfloat lightDiffuse[]  = { 1.0f, 1.0f, 0.50f, 1.0f };        
        const GLfloat lightPosition[] = { 0.0f, 0.0f, 50.0f, 0.0f };

        //Configure OpenGL lighting

        GLStates::lighting.Set(true);
        GLStates::light[0].enable.Set(true);
      //GLStates::light[1].enable.Set(true);
            
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
       // glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.001f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00f);
     //   const GLfloat lightDiffuse1[]  = { 1.8f, 1.8f, 1.8f, 1.0f };
     //   const GLfloat lightPosition1[] = { 0.0f, 0.0f, 100.0f, 1.0f };
     //   glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
     //   glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
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
void SetPointersInterleaved(void *data, unsigned vertices, bool normals) 
{
    memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);

    int size = 20;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, size, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)data + 12));    
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)data + 20));
}

// this is essentially for Win32 only
void SetPointersInterleavedShortPos(void *data, unsigned vertices, bool normals) 
{
    memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);

    int size = 16;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_SHORT, size, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)data + 8));    
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)data + 16));
}

// this is essentially for Win32 only
void SetPointersInterleavedCharPos(void *data, unsigned vertices, bool normals) 
{
    memset(&cachedPointers[0], 0, sizeof(cachedPointers));

    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);

    int size = 12;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_BYTE,  size, ((char*)data + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)data + 4));
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)data + 20));
}

void SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices, bool normals)
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

    int size = 20;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_FLOAT, size, ((char*)NULL + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)NULL + 12));    
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)NULL + 20));
    
    // This could actually be moved into the setup since we never disable it
    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);
    
    cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

void SetBuffersInterleavedCharPos(unsigned vb, unsigned ib, unsigned vertices, bool normals)
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

    int size = 12;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_SHORT, size, ((char*)NULL + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)NULL + 8));    
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)NULL + 16));
    
    // This could actually be moved into the setup since we never disable it
    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);
    
    cachedBuffers[0] = vb; cachedBuffers[1] = ib;
}

void SetBuffersInterleavedShortPos(unsigned vb, unsigned ib, unsigned vertices, bool normals)
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

    int size = 12;
    if (normals)
        size += 12;

    // Describe to OpenGL where the vertex data is in the buffer
    glVertexPointer  (3, GL_BYTE,  size, ((char*)NULL + 0));
    // Describe to OpenGL where the uv data is in the buffer
    glTexCoordPointer(2, GL_FLOAT, size, ((char*)NULL + 8));    
    // Describe to OpenGL where the normal data is in the buffer
    if (normals)
        glNormalPointer(GL_FLOAT, size, ((char*)NULL + 16));
    
    // This could actually be moved into the setup since we never disable it
    GLStates::vertices.Set(true);
    GLStates::normals.Set(normals);
    GLStates::uvs.Set(true);
    
    cachedBuffers[0] = vb; cachedBuffers[1] = ib;
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

void PrintTextureMatrix()
{
    Matrix m;
    glGetFloatv(GL_TEXTURE_MATRIX, (float *)&m);
    Print("texture", m);
}
