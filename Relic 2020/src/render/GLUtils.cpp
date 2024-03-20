#include "GLUtils.h"
#include "core/global.h"
#include "render/GLIncludes.h"
#include "render/GLStates.h"
#include "GLAbstract.h"

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
    GLLoadIdentityMatrix();

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
            
        GLSetLight4(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        GLSetLight4(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        GLSetLight4(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
     // GLSetLight4(GL_LIGHT0, GL_POSITION, lightPosition);
        GLSetLight(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.001f);
        GLSetLight(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        GLSetLight(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00f);
     // const GLfloat lightDiffuse1[]  = { 1.8f, 1.8f, 1.8f, 1.0f };
     // const GLfloat lightPosition1[] = { 0.0f, 0.0f, 100.0f, 1.0f };
     // GLSetLight4(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
     // GLSetLight4(GL_LIGHT1, GL_POSITION, lightPosition1);
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
                        
        GLSetLight(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.00001f);
        GLSetLight4(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        GLSetLight4(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        GLSetLight4(GL_LIGHT0, GL_POSITION, lightPosition);
        GLShadeModelSmooth();
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
            
        GLSetLight4(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        GLSetLight4(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        GLSetLight4(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
        GLSetLight4(GL_LIGHT0, GL_POSITION, lightPosition);
        GLSetLight(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001f);
        const GLfloat lightDiffuse1[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
        const GLfloat lightPosition1[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        GLSetLight4(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);
        GLSetLight4(GL_LIGHT1, GL_POSITION, lightPosition1);
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
        GLSetLight4(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        GLSetLight4(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        GLSetLight4(GL_LIGHT0, GL_POSITION, lightPosition);
    }
}

enum PointerType { VERTEX = 0, NORMAL, UV, MAX_POINTER_TYPES };

static void     *cachedPointers[MAX_POINTER_TYPES] = { 0 };
static unsigned  cachedBuffers[2] = { 0 };

void ClearBuffers()
{
    if (cachedBuffers[0] || cachedBuffers[1]) {
        GLBindBufferForElements(0, 0);
        cachedBuffers[0] = cachedBuffers[1] = 0;
    }
}

void ClearCachedPointers()
{
    memset(&cachedPointers[0], 0, sizeof(cachedPointers));
    ClearBuffers();
}

void SetBuffersInterleaved(unsigned vb, unsigned ib, unsigned vertices, bool normals)
{
    memset(&cachedPointers[0], 0, sizeof(cachedPointers));    
    
    if (!vb) {
        ClearBuffers();
        return;
    }
    
    if (vb == cachedBuffers[0] && ib == cachedBuffers[1]) {
        return;
    }

    // Activate the VBOs to draw
    GLBindBufferForElements(vb, ib);
    GLSetVertexPointersInterleaved(normals);

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

void PrintTextureMatrix()
{
    Matrix m;
    _GLv(glGetFloatv(GL_TEXTURE_MATRIX, (float *)&m));
    Print("texture", m);
}
