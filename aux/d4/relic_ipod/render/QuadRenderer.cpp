#include "QuadRenderer.h"
#include "platform/GLUtils.h"
#include "platform/GLIncludes.h"
#include "Material.h"
#include "OverheadCamera.h"
#include "DeviceTexture.h"
#include "GraphicsDevice.h"

extern RenderContext &GetGlobalRenderContext();

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

void QuadRenderer::RenderScreenAlignedQuad(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                           const Vector2 &t0, const Vector2 &t1)
{
//return; // $TODO ?
    if (color.w <= 0.01f)
        return;

    RenderContext &context = GetGlobalRenderContext();
    
    static Material material;
    material.ambient = Vector4(2.0f * color.x, 2.0f * color.y, 2.0f * color.z, 1.0f);
    material.diffuse = Vector4(1.0f, 1.0f, 1.0f, color.w);
    context.device.SetMaterial(material);
    context.device.EnableDepthTest(true);
    context.device.EnableDepthWrite(false);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((GLfloat *)(context.camera.GetView().data));        
    
    glDisable(GL_CULL_FACE);    
//  glDisable(GL_DEPTH_TEST);
    
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
    
    ClearCachedPointers();
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);    
    
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    
    glEnable(GL_BLEND);    
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    if (!texture || !texture->Loaded())
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);            
        glEnable(GL_TEXTURE_2D);
      //texture->Set(context.device, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);    
    context.device.EnableDepthWrite(true);
}

void QuadRenderer::RenderScreenQuad(DeviceTexture *texture, const Vector4 &color, float width, float height, const Vector3 &s, const Vector3 &t,
                                    const Vector2 &us, const Vector2 &uvt)
{
//return; // $TODO ?
    if (color.w <= 0.01f)
        return;

    RenderContext &context = GetGlobalRenderContext();
    
    static Material material;
    material.ambient = Vector4(2.0f * color.x, 2.0f * color.y, 2.0f * color.z, 1.0f);
    material.diffuse = Vector4(1.0f, 1.0f, 1.0f, color.w);
    context.device.SetMaterial(material);
    
    glLoadMatrixf((GLfloat *)(context.camera.GetView().data));    
    
    glDisable(GL_CULL_FACE);    
    glDisable(GL_DEPTH_TEST);    
    
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
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    
    glEnable(GL_BLEND);    
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    
    
    if (!texture || !texture->Loaded())
    {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glTexCoordPointer(2, GL_FLOAT, 0, uvs);            
        glEnable(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);        
        texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
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
    RenderScreenQuad(texture, color, 320.0f, 320.0f, Vector3(-scale, -scale, 1.0f), at);
}

void QuadRenderer::RenderTitle(DeviceTexture *texture, float alpha, float scale, float z)
{
    RenderContext &context = GetGlobalRenderContext();
    Vector3 at(context.camera.GetLookAt()); at.z = z;
    RenderScreenQuad(texture, Vector4(1.0f, 1.0f, 1.0f, alpha), 1024.0f, 256.0f, Vector3(-scale, -scale, 1.0f), at);
}