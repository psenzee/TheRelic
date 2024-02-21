/*
 *  GlyphWriter_DrawString.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "core/xna.h"

#include "Glyph.h"
#include "GLUtils.h"
#include "DeviceTexture.h"
#include "Material.h"

inline static void _append(const Vector3 &pos, float u, float v, unsigned color, float **ppos, float **puv, unsigned **pcolor)
{
    *((Vector3  *)*ppos)   = pos;    (*ppos) += 3;
    *((float    *)*puv)    = u;      (*puv)++;
    *((float    *)*puv)    = v;      (*puv)++;
    *((unsigned *)*pcolor) = color;  (*pcolor)++;    
}

void GlyphWriter::DrawString(const char *s, const Vector2 &lookAt, const Vector2 &at, float alpha, float scaler, const Matrix &view)
{
    static const Vector2 ZEROS(0.0f, 0.0f);
    static const Vector4 ONES (1.0f, 1.0f, 1.0f, 1.0f);
    
    Matrix ms, m;
    ms.scale(Vector3(scaler, scaler, 1.0f));
    m.translate(Vector3(lookAt.x + at.x, lookAt.y + at.y, 0.0f));
    m = ms * m;
    DrawString(s, ZEROS, ZEROS, ONES, m, view, buffer);
}

void GlyphWriter::DrawString(const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer)
{
    if (!s) return;

    // CREATE DRAW LIST

//    glDisable(GL_LIGHTING);
    
    Vector3              initial  = v3(start), position(v3(pos));
    unsigned int         clr      = 0xffffffff;
        
    static const Vector3 SCALE(-1.0f, -1.0f, 1.0f);
    static const Vector3 UL(Vector3(-0.5f, -0.5f, 0.0f) * SCALE), UR(Vector3( 0.5f, -0.5f, 0.0f) * SCALE),
                         LL(Vector3(-0.5f,  0.5f, 0.0f) * SCALE), LR(Vector3( 0.5f,  0.5f, 0.0f) * SCALE);
                         
    enum { VERTICES_PER_QUAD = 6 }; // for now, non-indexed, non-stripped

    size_t               glyphs    = strlen(s),
                         count     = glyphs * VERTICES_PER_QUAD,
                         bytes     = (count * 3 /* vertices */ + count * 2 /* uvs */) * sizeof(float) + count /* colors */ * sizeof(unsigned);
    
    void                *data      = buffer;

    if (!buffer || bytes > BUFFER_BYTES)
    {
        // if no buffer is supplied or it's too small        
        buffer = 0;
        data   = new unsigned [bytes / sizeof(unsigned)]; // 1 single allocation
    }

    float               *vertices  = ((float *)data),
                        *pvertices = vertices,
                        *uvs       = ((float *)data) + count * 3,
                        *puvs      = uvs;
    unsigned            *colors    = (unsigned *)(((float *)data) + count * 3 + count * 2),
                        *pcolors   = colors;

    unsigned             real_count = 0;
    
    while (*s)
    {    
        if (*s == '\n')
        {
            position.y += SCALE.y;
            position.x  = initial.x;
            s++;
        }
        else if (!ParseColor(&s, clr))
        {
            int c = *s - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;

            GlyphExtent &ex = extents[c];
            
            if (c != 0)  // zero is space, don't bother rendering it..
            {
                float u0 = ex.uv0.x, v0 = ex.uv0.y,
                      u1 = ex.uv1.x, v1 = ex.uv1.y;
                      
                _append(position + UL, u0, v0, clr, &pvertices, &puvs, &pcolors);
                _append(position + LL, u0, v1, clr, &pvertices, &puvs, &pcolors);
                _append(position + LR, u1, v1, clr, &pvertices, &puvs, &pcolors);
                
                _append(position + UL, u0, v0, clr, &pvertices, &puvs, &pcolors);
                _append(position + LR, u1, v1, clr, &pvertices, &puvs, &pcolors);
                _append(position + UR, u1, v0, clr, &pvertices, &puvs, &pcolors);
                
                real_count += VERTICES_PER_QUAD;
            }
    
            position.x += ex.width * SCALE.x;
            s++;
        }
    }

    // RENDER DRAW LIST
        
    static Vector4  zeros(0.0f, 0.0f, 0.0f, 1.0f);
    static Material m(zeros, zeros, zeros, zeros, 0.0f);
    
    ClearCachedPointers();
    texture->Set(*(services->device), GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m.ambient = color * Vector4(0.20f, 0.20f, 0.20f, 0.20f);
//    m.ambient = zeros;
    m.diffuse = Vector4(1.0f, 1.0f, 1.0f, color.w);    
    m.Set(*(services->device));

    glDisableClientState(GL_NORMAL_ARRAY);    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDisable(GL_CULL_FACE);  // we can eliminate this if we ensure the correct orientation of the vertices
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);    
    
    glVertexPointer  (3, GL_FLOAT,         0, (GLfloat *)&vertices[0]);
    glTexCoordPointer(2, GL_FLOAT,         0, (GLfloat *)&uvs[0]);
    glColorPointer   (4, GL_UNSIGNED_BYTE, 0, (GLfloat *)&colors[0]);    

    glLoadMatrixf((GLfloat *)view.data);
    glMultMatrixf((GLfloat *)transform.data);

    glDrawArrays(GL_TRIANGLES, 0, real_count);
    
    glDisableClientState(GL_COLOR_ARRAY);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);    
    glEnable(GL_CULL_FACE); // we can eliminate this if we ensure the correct orientation of the vertices    
    
//    glEnable(GL_LIGHTING);    
    
    if (!buffer)
        delete [] (unsigned *)data;
}

/*

void GameUI::DrawString(const char *s, const Vector2 &center, const Vector2 &at, float alpha, float scaler)
{
    Text *text = new Text(color ? colorwriter : writer, s,
                          Vector2(0.0f, 0.0f), scale,
                          Vector3(1.0f, 1.0f, 1.0f), 0);
    Matrix ms, m;
    ms.scale(Vector3(scaler, scaler, 1.0f));
    m.translate(Vector3(center.x + at.x, center.y + at.y, 0.0f));
    m = ms * m;
    glDisable(GL_DEPTH_TEST);
    text->Render(m, *(state->camera), Vector4(1.0f, 1.0f, 1.0f, alpha));
    glEnable(GL_DEPTH_TEST);
    delete text;
}

Vector2 GameUI::GetSize(const char *s)
{
    size_t dsz = strlen(s);
    char *t = new char [dsz + 1];
    Text::StripColor(s, t);
    Vector2 sz = writer->GetSize(t, scale);
    delete [] t;
    return sz;
}

AABox GameUI::GetCenteredHitTestBox(const char *s, const Vector2 &at, float scaler)
{
    float hs = scaler * 0.5f;
    Vector2 sz(GetSize(s) * hs);
    return AABox(Vector3(sz.x + at.x, at.y + hs, 0.0f), Vector3(-sz.x + hs + at.x, at.y - hs, 0.0f));
}

void GameUI::DrawStringCentered(const char *s, const Vector2 &at, float alpha, float scaler, bool color)
{
    float hs = scaler * 0.5f;    
    Vector2 sz(GetSize(s) * hs), point(sz.x + at.x, at.y);
    DrawString(s, point, alpha, scaler, color);
}

*/