#include "core/core.h"
#include "core/random.h"
#include "core/strs.h"

#include "Glyph.h"
#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "platform/GLIncludes.h"
#include "render/DeviceTexture.h"
#include "render/GraphicsDevice.h"
#include "render/Material.h"
#include "render/RenderContext.h"

extern void ClearCachedPointers();

inline static void _append(const Vector3 &pos, float u, float v, float **ppos, float **puv)
{
    *((Vector3  *)*ppos)   = pos;    (*ppos) += 3;
    *((float    *)*puv)    = u;      (*puv)++;
    *((float    *)*puv)    = v;      (*puv)++;
}

void GlyphWriter::DrawString(RenderContext &context, const char *s, const Vector2 &lookAt, const Vector2 &at, float alpha, float size, const Matrix &view, bool useInlineColor, TextJustify justify)
{
    DrawString(context, s, lookAt, at, Vector4(1.0f, 1.0f, 1.0f, alpha), size, view, useInlineColor, justify);
}

void GlyphWriter::DrawString(RenderContext &context, const char *s, const Vector2 &lookAt, const Vector2 &at, const Vector4 &color, float size, const Matrix &view, bool useInlineColor, TextJustify justify)
{
    static const Vector2 ZEROS(0.0f, 0.0f);
    static const Vector4 ONES (1.0f, 1.0f, 1.0f, 1.0f);
    //printf("gw:drawstring look at (%f, %f) at (%f, %f) '%s' \n", lookAt.x, lookAt.y, at.x, at.y, s);
    Matrix ms, m;
    ms.scale(Vector3(size, size, 1.0f));
    m.translate(Vector3(lookAt.x + at.x, lookAt.y + at.y, 0.0f));
    m = ms * m;
    DrawString(context, s, Vector2(-0.5f, 0.0f), Vector2(-0.5f, 0.0f), color, m, view, buffer, useInlineColor, justify);
}

static void _RenderString(RenderContext &context, float *vertices, float *uvs, int count, const Vector4 &color)
{
    if (!count || !vertices || !uvs || color.w < 0.1f) return;
        
    // RENDER DRAW LIST
    //glColor4f(color.x, color.y, color.z, color.w);
    GraphicsDevice::GetInstance()->SetColor(color);
    /*
    Vector4 ambient(color * Vector4(2.0f, 2.0f, 2.0f, 0.0f)),
            diffuse(1.0f, 1.0f, 1.0f, color.w);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *)&ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *)&diffuse);
    */

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLfloat *)&vertices[0]);
    glEnableVertexAttribArray(0);
  //glTexCoordPointer(2, GL_FLOAT,         0, (GLfloat *)&uvs[0]);

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

quad_t unrotated_point;
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

void GlyphWriter::DrawString(RenderContext &context, const char *s, const Vector2 &start, const Vector2 &pos, const Vector4 &color, const Matrix &transform, const Matrix &view, void *buffer, bool useInlineColor, TextJustify justify)
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
        unrotated_point.rotate(0.f);
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
        data   = new uint32_t [bytes / sizeof(uint32_t)]; // 1 single allocation
    }

    float               *vertices  = ((float *)data),
                        *pvertices = vertices,
                        *uvs       = ((float *)data) + count * 3,
                        *puvs      = uvs;
    uint32_t             vcount    = 0;

    //glLoadMatrixf((GLfloat *)view.data);
    //glMultMatrixf((GLfloat *)transform.data);

    ClearCachedPointers();
    texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glDisableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDisable(GL_CULL_FACE);  // we can eliminate this if we ensure the correct orientation of the vertices
    //GLStates::depthWrite.Set(false);
    //GLStates::depthTest.Set(false);
    
    Vector4 scolor(1.0f, 1.0f, 1.0f, 1.0f), prev(scolor);

    const char *newlinestart = s;

    switch (justify)
    {
    case TEXT_JUSTIFY_LEFT:
        break;
    case TEXT_JUSTIFY_CENTER:
        {
            Vector2 size = GetSize(newlinestart, Vector2(-1.f, -1.f), true);
            position.x += size.x * 0.5f;
        }
        break;
    case TEXT_JUSTIFY_RIGHT:
        {
            Vector2 size = GetSize(newlinestart, Vector2(-1.f, -1.f), true);
            position.x += size.x;
        }
        break;
    }
    
    while (*s)
    {   
        if (*s == '\n')
        {
            position.y += SCALE.y;
            position.x  = initial.x;
            s++;
            newlinestart = s;
            switch (justify)
            {
            case TEXT_JUSTIFY_LEFT:
                break;
            case TEXT_JUSTIFY_CENTER:
                {
                    Vector2 size = GetSize(newlinestart, Vector2(-1.f, -1.f), true);
                    position.x += size.x * 0.5f;
                }
                break;
            case TEXT_JUSTIFY_RIGHT:
                {
                    Vector2 size = GetSize(newlinestart, Vector2(-1.f, -1.f), true);
                    position.x += size.x;
                }
                break;
            }
        }
        else if (ParseColor(&s, scolor) && !_hack_override_color)
        {
            // flush
            _RenderString(context, vertices, uvs, vcount, useInlineColor ? (prev * color) : color);
            pvertices = vertices;
            puvs      = uvs;
            vcount    = 0;
            prev      = scolor;
        }   
        else
        {
            int c = (unsigned char)*s - GLYPH_START;
            if (c < 0 || c >= MAX_GLYPHS)
                c = 0;

            GlyphExtent &ex = extents[c];
            
            if (c != 0)  // zero is space, don't bother rendering it..
            {
                float u0 = ex.uv0.x, v0 = ex.uv0.y,
                      u1 = ex.uv1.x, v1 = ex.uv1.y;
                
                quad_t &q = /*c > (128 - GLYPH_START) ? */unrotated_point; //: rotated_points[char_index[c]]; // hacked to not rotate glyphs > 128
                
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
    {
        if (_hack_override_color)
            _RenderString(context, vertices, uvs, vcount, *_hack_override_color);
        else
        _RenderString(context, vertices, uvs, vcount, prev * color);  
    }

    //GLStates::depthWrite.Set(true);
    //GLStates::depthTest.Set(true);
    glEnable(GL_CULL_FACE); // we can eliminate this if we ensure the correct orientation of the vertices   
    
    if (!buffer)
        delete [] (unsigned *)data;
}
