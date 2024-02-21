/*
 *  GlyphWriter_DrawString.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/31/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "core/xna.h"
#include "core/random.h"

#include "Glyph.h"
#include "GLUtils.h"
#include "DeviceTexture.h"
#include "LocalGameServices.h"
#include "Material.h"

inline static void _append(const Vector3 &pos, float u, float v, float **ppos, float **puv)
{
    *((Vector3  *)*ppos)   = pos;    (*ppos) += 3;
    *((float    *)*puv)    = u;      (*puv)++;
    *((float    *)*puv)    = v;      (*puv)++;
}

void GlyphWriter::DrawString(const char *s, const Vector2 &lookAt, const Vector2 &at, float alpha, float size, const Matrix &view)
{
    static const Vector2 ZEROS(0.0f, 0.0f);
    static const Vector4 ONES (1.0f, 1.0f, 1.0f, 1.0f);
    
    Matrix ms, m;
    ms.scale(Vector3(size, size, 1.0f));
    m.translate(Vector3(lookAt.x + at.x, lookAt.y + at.y, 0.0f));
    m = ms * m;
    DrawString(s, Vector2(-0.5f, 0.0f), Vector2(-0.5f, 0.0f), Vector4(1.0f, 1.0f, 1.0f, alpha), m, view, buffer);
}

static void _RenderString(float *vertices, float *uvs, int count, const Vector4 &color)
{
    if (!count || !vertices || !uvs) return;
        
    // RENDER DRAW LIST
    
    Vector4 ambient(color * Vector4(2.0f, 2.0f, 2.0f, 0.0f)),
            diffuse(1.0f, 1.0f, 1.0f, color.w);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *)&ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *)&diffuse);
    
    glVertexPointer  (3, GL_FLOAT,         0, (GLfloat *)&vertices[0]);
    glTexCoordPointer(2, GL_FLOAT,         0, (GLfloat *)&uvs[0]);

    glDrawArrays(GL_TRIANGLES, 0, count);
}

struct quad_t
{
    Vector3 point[4];
    
    void rotate(float x)
    {
        static const Vector3 SCALE(-1.0f, -1.0f, 1.0f);
        static const Vector3 POINTS[] = { Vector3(-0.5f, -0.5f, 0.0f) * SCALE, 
                                          Vector3( 0.5f, -0.5f, 0.0f) * SCALE,
                                          Vector3(-0.5f,  0.5f, 0.0f) * SCALE, 
                                          Vector3( 0.5f,  0.5f, 0.0f) * SCALE };
        Matrix m;
        m.rotationz(x);
        for (int i = 0; i < 4; i++)
            point[i] = m * POINTS[i];        
    }
};

quad_t rotated_points[16];
int    char_index[256];

void GlyphWriter::Update()
{
    for (int i = 0; i < 256; i++)
    {
        char_index[i]++;
        char_index[i] %= 16;
    }
}

void GlyphWriter::DrawString(const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer)
{
    if (!s || !*s || color.w < 0.01f) return;
    
    Vector3              initial  = v3(start), position(v3(pos));
        
    static const Vector3 SCALE(-1.0f, -1.0f, 1.0f);
    static const Vector3 UL(Vector3(-0.5f, -0.5f, 0.0f) * SCALE), UR(Vector3( 0.5f, -0.5f, 0.0f) * SCALE),
                         LL(Vector3(-0.5f,  0.5f, 0.0f) * SCALE), LR(Vector3( 0.5f,  0.5f, 0.0f) * SCALE);
    
    static bool inited = false;
    
    if (!inited)
    {
        inited = true;
        for (int i = 0; i < 16; i++)
            rotated_points[i].rotate((float)(sin(2.0 * 3.141592653589793 * i / 16.0)) * 0.1f);
        for (int i = 0; i < 256; i++)
            char_index[i] = rand() % 16;
    }
       
    enum { VERTICES_PER_QUAD = 6 }; // for now, non-indexed, non-stripped

    size_t               glyphs    = strlen(s),
                         count     = glyphs * VERTICES_PER_QUAD,
                         bytes     = (count * 3 /* vertices */ + count * 2 /* uvs */) * sizeof(float);
    
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
    unsigned             vcount    = 0;

    glLoadMatrixf((GLfloat *)view.data);
    glMultMatrixf((GLfloat *)transform.data);

    ClearCachedPointers();
    texture->Set(*(services->device), GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisableClientState(GL_NORMAL_ARRAY);  
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_CULL_FACE);  // we can eliminate this if we ensure the correct orientation of the vertices
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    
    Vector4 scolor(1.0f, 1.0f, 1.0f, 1.0f), prev(scolor);
    
    while (*s)
    {   
        if (*s == '\n')
        {
            position.y += SCALE.y;
            position.x  = initial.x;
            s++;
        }
        else if (ParseColor(&s, scolor))
        {
            // flush
            _RenderString(vertices, uvs, vcount, prev * color);
            pvertices = vertices;
            puvs      = uvs;
            vcount    = 0;
            prev      = scolor;
        }   
        else
        {
            int c = *s - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;

            GlyphExtent &ex = extents[c];
            
            if (c != 0)  // zero is space, don't bother rendering it..
            {
                float u0 = ex.uv0.x, v0 = ex.uv0.y,
                      u1 = ex.uv1.x, v1 = ex.uv1.y;
                /*
                _append(position + UL, u0, v0, &pvertices, &puvs);
                _append(position + LL, u0, v1, &pvertices, &puvs);
                _append(position + LR, u1, v1, &pvertices, &puvs);
                
                _append(position + UL, u0, v0, &pvertices, &puvs);
                _append(position + LR, u1, v1, &pvertices, &puvs);
                _append(position + UR, u1, v0, &pvertices, &puvs);
                */
                
                quad_t &q = rotated_points[char_index[c]];
                
                _append(position + q.point[0], u0, v0, &pvertices, &puvs);
                _append(position + q.point[2], u0, v1, &pvertices, &puvs);
                _append(position + q.point[3], u1, v1, &pvertices, &puvs);
                
                _append(position + q.point[0], u0, v0, &pvertices, &puvs);
                _append(position + q.point[3], u1, v1, &pvertices, &puvs);
                _append(position + q.point[1], u1, v0, &pvertices, &puvs);
                
                vcount += VERTICES_PER_QUAD;
            }
    
            position.x += ex.width * SCALE.x;
            s++;
        }
    }

    if (vcount) 
        _RenderString(vertices, uvs, vcount, prev * color);  
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);   
    glEnable(GL_CULL_FACE); // we can eliminate this if we ensure the correct orientation of the vertices   
    
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