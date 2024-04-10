#include "GlyphDrawList.h"
#include "render/GLUtils.h"
#include "render/GLStates.h"
#include "render/GLIncludes.h"
#include "core/global.h"

#include "render/OverheadCamera.h"
#include "render/DeviceTexture.h"
#include "render/Material.h"
#include "render/RenderContext.h"
#include "render/GraphicsDevice.h"

GlyphDrawList::GlyphDrawList(DeviceTexture *texture, const Vector3 &scale, const Vector4 &color) 
    : scale(scale), color(color), texture(texture), maximum(FLT_MIN, FLT_MIN, FLT_MIN), minimum(FLT_MAX, FLT_MAX, FLT_MAX)
{
}

void GlyphDrawList::Add(const Glyph &glyph)
{
    Vector3 p(glyph.position.x, glyph.position.y, 0.0f),
            s(glyph.scale.x,    glyph.scale.y,    1.0f);
    
    static const Vector3 UL(-0.5f, -0.5f, 0.0f), UR( 0.5f, -0.5f, 0.0f),
                         LL(-0.5f,  0.5f, 0.0f), LR( 0.5f,  0.5f, 0.0f);
    
    float u0 = glyph.extent.uv0.x, v0 = glyph.extent.uv0.y,
          u1 = glyph.extent.uv1.x, v1 = glyph.extent.uv1.y;

    vertices.push_back(p + UL * s); uvs.push_back(Vector2(u0, v0)); colors.push_back(glyph.color);
    vertices.push_back(p + LL * s); uvs.push_back(Vector2(u0, v1)); colors.push_back(glyph.color);
    vertices.push_back(p + LR * s); uvs.push_back(Vector2(u1, v1)); colors.push_back(glyph.color);
            
    vertices.push_back(p + UL * s); uvs.push_back(Vector2(u0, v0)); colors.push_back(glyph.color);
    vertices.push_back(p + LR * s); uvs.push_back(Vector2(u1, v1)); colors.push_back(glyph.color);
    vertices.push_back(p + UR * s); uvs.push_back(Vector2(u1, v0)); colors.push_back(glyph.color);
}

void GlyphDrawList::SetCursor(const Vector3 &p)
{
    cursor = p;
    if (minimum.x > maximum.x)
        initial = cursor;    
    minimum = minimum.minimum(p);
    maximum = maximum.maximum(p);
}

void GlyphDrawList::Render(RenderContext &context, const Matrix &transform, const Vector4 &color)
{
    Vector4 clr(color * this->color);
    if (clr.w < 0.1f)
        return;
    
    texture->Set(context.device, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    ClearCachedPointers();

    GLSetEnabled(GL_CULL_FACE, false);

    //static Vector4  zeros(0.0f, 0.0f, 0.0f, 1.0f);
    /*
    static Material m(zeros, zeros, zeros, zeros, 0.0f);
    m.ambient = clr * Vector4(2.0f, 2.0f, 2.0f, 0.0f);
    m.diffuse = Vector4(1.0f, 1.0f, 1.0f, clr.w);        
    
    context.device.SetMaterial(m);
    */
    GraphicsDevice::GetInstance()->SetColor(clr);
    
    GLSetEnabledClientState(GL_VERTEX_ARRAY, true);
    GLSetEnabledClientState(GL_NORMAL_ARRAY, false);
    GLSetEnabledClientState(GL_TEXTURE_COORD_ARRAY, true);
    GLStates::depthWrite.Set(false);
    GLStates::depthTest.Set(false);    
    
    GLSetVertexPointer((const float *)vertices.data());
    GLSetTexCoordPointer((const float *)uvs.data());

    //GLLoadMatrix(context.camera.GetView());
    //GLMultMatrix(transform.data);
    GLLoadMatrixStack(
        context.camera.GetProjection(),
        context.camera.GetView(),
        transform
    );
    
    //_GLv(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size()));
    GLDrawArrays(GL_TRIANGLES, vertices.size());
    
    GLStates::depthWrite.Set(true);
    GLStates::depthTest.Set(true);    
}
