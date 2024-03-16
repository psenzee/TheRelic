#include "QuadRenderer.h"
#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "platform/GLIncludes.h"
#include "Material.h"
#include "OverheadCamera.h"
#include "DeviceTexture.h"
#include "GraphicsDevice.h"

extern RenderContext &GetGlobalRenderContext();

std::vector<ScreenQuadToRender> QuadRenderer::sDeferred;

void QuadRenderer::RenderQuads()
{
    for (std::map<std::string, ScreenQuadToRender>::iterator i = quads.begin(), e = quads.end(); i != e; ++i)
    {
        ScreenQuadToRender &quad = (*i).second;
        RenderScreenAlignedQuad(quad.texture, quad.color, quad.start, quad.end, quad.t0, quad.t1);
    }
}

void QuadRenderer::Set(const char *name, ScreenQuadToRender &quad)
{
    quads[name] = quad;
}

void QuadRenderer::RenderScreenAlignedQuadDeferred(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                                   const Vector2 &t0, const Vector2 &t1)
{
    if (color.w <= 0.01f)
        return;

    ScreenQuadToRender quad;
    quad.texture = texture;
    quad.color   = color;
    quad.start   = start;
    quad.end     = end;
    quad.t0      = t0;
    quad.t1      = t1;
    sDeferred.push_back(quad);
}

void QuadRenderer::RenderDeferred()
{
    for (std::vector<ScreenQuadToRender>::iterator i = sDeferred.begin(), e = sDeferred.end(); i != e; ++i)
        RenderScreenAlignedQuad((*i).texture, (*i).color, (*i).start, (*i).end, (*i).t0, (*i).t1);
    sDeferred.clear();
}

void QuadRenderer::RenderScreenAlignedQuad(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                           const Vector2 &t0, const Vector2 &t1)
{
    if (color.w <= 0.01f)
        return;

    float vertices[] =
    { 
        start.x,  end.y,    start.z,   //     t0.x, t1.y,
        end.x,    start.y,  start.z,   //     t1.x, t0.y,
        start.x,  start.y,  start.z,   //     t0.x, t0.y,
        
        start.x,  end.y,    start.z,   //     t0.x, t1.y,
        end.x,    end.y,    start.z,   //     t1.x, t1.y,
        end.x,    start.y,  start.z,   //     t1.x, t0.y,
    };
    
    float uvs[] =
    {
        t0.x, t1.y,
        t1.x, t0.y,
        t0.x, t0.y,
        
        t0.x, t1.y,
        t1.x, t1.y,
        t1.x, t0.y,
    };

    RenderScreenAligned(texture, color, vertices, uvs, 6);
}

void QuadRenderer::RenderScreenAlignedQuadRotate(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                                const Vector2 &t0, const Vector2 &t1, float angle)
{
    if (color.w <= 0.01f)
        return;

    float vertices[] =
    { 
        start.x,  end.y,    start.z,
        end.x,    start.y,  start.z,        
        start.x,  start.y,  start.z,
        
        start.x,  end.y,    start.z,        
        end.x,    end.y,    start.z,                
        end.x,    start.y,  start.z
    };
    
    float uvs[] =
    {
        t0.x, t1.y,
        t1.x, t0.y,
        t0.x, t0.y,
        
        t0.x, t1.y,
        t1.x, t1.y,
        t1.x, t0.y        
    };

    Vector3 center((start + end) * 0.5f);
    Matrix m;
    m.translate(-center);
    Matrix mn;
    mn.rotationz(angle);
    m = m * mn;
    mn.translate(center);
    m = m * mn;
    for (int i = 0; i < 6 * 3; i += 3)
    {
        Vector3 v(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
        v = m * v;
        vertices[i + 0] = v.x;
        vertices[i + 1] = v.y; 
        vertices[i + 2] = v.z;
    }

    RenderScreenAligned(texture, color, vertices, uvs, 6);
}

void QuadRenderer::RenderScreenAlignedPanoramaRotate(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end, float angle)
{
    if (color.w <= 0.01f)
        return;

    float xsz0 = 0.f,
          xsz1 = (end.x - start.x) * 0.25f,
          xsz2 = xsz1 + xsz1,
          xsz3 = xsz2 + xsz1,
          xsz4 = xsz3 + xsz1;

    float vertices[] =
    {
        // Q1
        start.x + xsz0,  end.y,    start.z,
        start.x + xsz1,  start.y,  start.z,
        start.x + xsz0,  start.y,  start.z,
        
        start.x + xsz0,  end.y,    start.z,        
        start.x + xsz1,  end.y,    start.z,                
        start.x + xsz1,  start.y,  start.z,

        // Q2
        start.x + xsz1,  end.y,    start.z,
        start.x + xsz2,  start.y,  start.z,
        start.x + xsz1,  start.y,  start.z,
        
        start.x + xsz1,  end.y,    start.z,        
        start.x + xsz2,  end.y,    start.z,                
        start.x + xsz2,  start.y,  start.z,

        // Q3
        start.x + xsz2,  end.y,    start.z,
        start.x + xsz3,  start.y,  start.z,
        start.x + xsz2,  start.y,  start.z,
        
        start.x + xsz2,  end.y,    start.z,        
        start.x + xsz3,  end.y,    start.z,                
        start.x + xsz3,  start.y,  start.z,

        // Q4
        start.x + xsz3,  end.y,    start.z,
        start.x + xsz4,  start.y,  start.z,
        start.x + xsz3,  start.y,  start.z,
        
        start.x + xsz3,  end.y,    start.z,        
        start.x + xsz4,  end.y,    start.z,                
        start.x + xsz4,  start.y,  start.z,
    };

    float uvs[] =
    {
        // Q1
        0.f, 0.75f,
        1.f, 1.f,
        0.f, 1.f,
        
        0.f, 0.75f,
        1.f, 0.75f,
        1.f, 1.f,

        // Q2
        0.f, 0.5f,
        1.f, 0.75f,
        0.f, 0.75f,
        
        0.f, 0.5f,
        1.f, 0.5f,
        1.f, 0.75f,

        // Q3
        0.f, 0.25f,
        1.f, 0.5f,
        0.f, 0.5f,
        
        0.f, 0.25f,
        1.f, 0.25f,
        1.f, 0.5f,

        // Q4
        0.f, 0.f,
        1.f, 0.25f,
        0.f, 0.25f,
        
        0.f, 0.f,
        1.f, 0.f,
        1.f, 0.25f,
    };

    if (fabsf(angle > 0.001f))
    {
        Vector3 center((start + end) * 0.5f);
        Matrix m;
        m.translate(-center);
        Matrix mn;
        mn.rotationz(angle);
        m = m * mn;
        mn.translate(center);
        m = m * mn;
        for (int i = 0; i < 6 * 4 * 3; i += 3)
        {
            Vector3 v(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
            v = m * v;
            vertices[i + 0] = v.x;
            vertices[i + 1] = v.y; 
            vertices[i + 2] = v.z;
        }
    }

    RenderScreenAligned(texture, color, vertices, uvs, 6 * 4);
}

static void CopyQuadTo(float *vertices, const Vector3 &start, const Vector3 &end, const Vector2 &t0, const Vector2 &t1)
{
    float qvertices[] =
    { 
        start.x,  end.y,    start.z,        t0.x, t1.y,
        end.x,    start.y,  start.z,        t1.x, t0.y,
        start.x,  start.y,  start.z,        t0.x, t0.y,
        
        start.x,  end.y,    start.z,        t0.x, t1.y,
        end.x,    end.y,    start.z,        t1.x, t1.y,
        end.x,    start.y,  start.z,        t1.x, t0.y,
    };

    memcpy(vertices, qvertices, sizeof(qvertices));
}

static void CopyQuadTo(float *vertices, const Vector3 &start, const Vector3 &end, const Vector2 &t0, const Vector2 &t1)
{
    float qvertices[] =
    {
        start.x,  end.y,    start.z,        t0.x, t1.y,
        end.x,    start.y,  start.z,        t1.x, t0.y,
        start.x,  start.y,  start.z,        t0.x, t0.y,
        
        start.x,  end.y,    start.z,        t0.x, t1.y,
        end.x,    end.y,    start.z,        t1.x, t1.y,
        end.x,    start.y,  start.z,        t1.x, t0.y,
    };

    memcpy(vertices, qvertices, sizeof(qvertices));
}

inline float mix(float a, float b, float t)
{
    return (b - a) * t + a;
}
/*
static size_t IndexedAAQuad(const Vector3 &vb, const Vector3 &ve, const Vector2 &tb, const Vector2 &te)
{
    // degenerate -- it's a point
    if (math::zero(vb.manhattan(ve))) {
        return 0;
    }
    bool eq_x = math::eq(vb.x, ve.x),
         eq_y = math::eq(vb.y, ve.y),
         eq_z = math::eq(vb.z, ve.z);
    // degenerate -- it's a line
    if ((eq_x && eq_y) || (eq_y && eq_z) || (eq_x && eq_z)) {
        return 0;
    } else if (eq_z || eq_y) { // I only know that eq_z has the correct normals.  I will have to test the rest
        const float vertices [] = {
            ve.x, vb.y, vb.z, te.x, tb.y,
            ve.x, ve.y, ve.z, te.x, te.y,
            vb.x, vb.y, vb.z, tb.x, tb.y,
            vb.x, ve.y, ve.z, tb.x, te.y,
        };
    } else if (eq_x) {
        const float vertices [] = {
            ve.x, ve.y, vb.z, te.x, tb.y,
            vb.x, ve.y, ve.z, te.x, te.y,
            ve.x, vb.y, vb.z, tb.x, tb.y,
            vb.x, vb.y, ve.z, tb.x, te.y,
        };
    }
    const uint16_t indices [] = {
        0, 1, 2, 2, 1, 3
    };
    return 6;
}
*/

void QuadRenderer::Render9Slice(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end, float cornerSize, float middleRange)
{
    float   csize = cornerSize,
            n = middleRange, m = 1.f - n;
    Vector3 csizes(csize, csize, 0.f);
    float   vertices[9 * 6 * 3], uvs[9 * 6 * 2],
            *pv = vertices, *pu = uvs;
    int     vsz = 6 * 3, usz = 6 * 2;

    CopyQuadTo(pv, pu, start,                                        start + csizes,                               Vector2(0.00f, 0.00f), Vector2(n, n)); pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, end - csizes,                                 end,                                          Vector2(m, m), Vector2(1.00f, 1.00f)); pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, Vector3(end.x - csize, start.y, 0.f),         Vector3(end.x, start.y + csize, 0.f),         Vector2(m, 0.00f), Vector2(1.00f, n)); pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, Vector3(start.x, end.y - csize, 0.f),         Vector3(start.x + csize, end.y, 0.f),         Vector2(0.00f, m), Vector2(n, 1.00f)); pv += vsz; pu += usz;

    CopyQuadTo(pv, pu, Vector3(start.x + csize, start.y, 0.f),       Vector3(end.x - csize, start.y + csize, 0.f), Vector2(n, 0.00f), Vector2(m, n));     pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, Vector3(start.x, start.y + csize, 0.f),       Vector3(start.x + csize, end.y - csize, 0.f), Vector2(0.00f, n), Vector2(n, m));     pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, Vector3(start.x + csize, end.y - csize, 0.f), Vector3(end.x - csize, end.y, 0.f),           Vector2(n, m), Vector2(m, 1.00f));     pv += vsz; pu += usz;
    CopyQuadTo(pv, pu, Vector3(end.x - csize, start.y + csize, 0.f), Vector3(end.x, end.y - csize, 0.f),           Vector2(m, n), Vector2(1.00f, m));     pv += vsz; pu += usz;

    CopyQuadTo(pv, pu, start + csizes,                               end - csizes,                                 Vector2(n, n), Vector2(m, m));         pv += vsz; pu += usz;

    RenderScreenAligned(texture, color, vertices, uvs, 6 * 9);
}

void QuadRenderer::RenderScreenAligned(DeviceTexture *texture, const Vector4 &color, const float *vertices, const float *uvs, int count)
{
    if (color.w <= 0.01f)
        return;

    RenderContext &context = GetGlobalRenderContext();

    context.device.SetColor(color);
    context.device.EnableDepthTest(true);
    context.device.EnableDepthWrite(false);
    
    ShaderProgram *p = context.device.SetShaderProgram("ScreenQuad");
    p->SetUniform("u_view", context.camera.GetView());

    glDisable(GL_CULL_FACE);

    ClearCachedPointers();

    //glEnableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);

    glEnable(GL_BLEND);    

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!texture || !texture->Loaded())
    {
        //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        //glTexCoordPointer(2, GL_FLOAT, 0, uvs);
        glEnable(GL_TEXTURE_2D);
      //texture->Set(context.device, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDrawArrays(GL_TRIANGLES, 0, count);
    glEnable(GL_CULL_FACE);
    context.device.EnableDepthWrite(true);
}

void QuadRenderer::RenderScreenQuad(DeviceTexture *texture, const Vector4 &color, float width, float height, const Vector3 &s, const Vector3 &t,
                                    const Vector2 &us, const Vector2 &uvt)
{
    if (color.w <= 0.01f)
        return;

    RenderContext &context = GetGlobalRenderContext();
    GraphicsDevice::GetInstance()->SetColor(color); // clear color
    
    //glLoadMatrixf((GLfloat *)(context.camera.GetView().data));
    
    glDisable(GL_CULL_FACE);    
    GLStates::depthTest.Set(false);    
    
    float halfw = width  * 0.5f, halfh = height * 0.5f;
    
    float vertices[] =
    { 
        -halfw * s.x + t.x,  halfh * s.y + t.y, t.z,
         halfw * s.x + t.x, -halfh * s.y + t.y, t.z,
        -halfw * s.x + t.x, -halfh * s.y + t.y, t.z,
        
        -halfw * s.x + t.x,  halfh * s.y + t.y, t.z,
         halfw * s.x + t.x,  halfh * s.y + t.y, t.z,
         halfw * s.x + t.x, -halfh * s.y + t.y, t.z
    };
    
    float uvs[] =
    {
        0.0f * us.x + uvt.x, 1.0f * us.y + uvt.y,
        1.0f * us.x + uvt.x, 0.0f * us.y + uvt.y,
        0.0f * us.x + uvt.x, 0.0f * us.y + uvt.y,
        
        0.0f * us.x + uvt.x, 1.0f * us.y + uvt.y,
        1.0f * us.x + uvt.x, 1.0f * us.y + uvt.y,
        1.0f * us.x + uvt.x, 0.0f * us.y + uvt.y        
    };
    
    ClearCachedPointers();
    
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(0);
    
    glEnable(GL_BLEND);    
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
    
    if (!texture || !texture->Loaded())
    {
        //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        //glTexCoordPointer(2, GL_FLOAT, 0, uvs);
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    GLStates::depthTest.Set(true); 
    glEnable(GL_CULL_FACE);    
}

void QuadRenderer::RenderBackground(DeviceTexture *texture, float alpha)
{
    RenderContext &context = GetGlobalRenderContext();
    Vector3 at(context.camera.GetLookAt()); at.z = 500.0f;
    RenderScreenQuad(texture, Vector4(1.0f, 1.0f, 1.0f, alpha),
                     3200.0f, 3200.0f,
                     Vector3(-1.0f, -1.0f, 1.0f), at, Vector2(2.0f, 2.0f), Vector2(-0.5f, -0.5f));    
}

void QuadRenderer::RenderOverlay(DeviceTexture *texture, const Vector4 &color, float scale)
{
    RenderContext &context = GetGlobalRenderContext();
    Vector3 at(context.camera.GetLookAt()); at.z = -100.0f;
    RenderScreenQuad(texture, color, 512.0f, 512.0f, Vector3(-scale, -scale, 1.0f), at);
}

void QuadRenderer::RenderTitle(DeviceTexture *texture, float alpha, float scale, float z)
{
    RenderContext &context = GetGlobalRenderContext();
    Vector3 at(context.camera.GetLookAt()); at.z = z;
    RenderScreenQuad(texture, Vector4(1.0f, 1.0f, 1.0f, alpha), 1024.0f, 256.0f, Vector3(-scale, -scale, 1.0f), at);
}
