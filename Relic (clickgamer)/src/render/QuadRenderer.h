#ifndef _QUADRENDERER_H
#define _QUADRENDERER_H

#include "core/core.h"
#include "RenderContext.h"

#include <map>
#include <string>

class DeviceTexture;

struct ScreenQuadToRender
{
    DeviceTexture *texture;
    Vector4        color;
    Vector3        start;
    Vector3        end;
    Vector2        t0,
                   t1;
};

class QuadRenderer
{
    static std::vector<ScreenQuadToRender> sDeferred;

public:
   
//  inline QuadRenderer(const RenderContext &context) : context(context) {}

    static void RenderScreenQuad(DeviceTexture *texture, const Vector4 &color, float width, float height, const Vector3 &s, const Vector3 &t,
                                                  const Vector2 &uvs = Vector2(1.0f, 1.0f), const Vector2 &uvt = Vector2(0.0f, 0.0f));
    static void RenderScreenAlignedQuad(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                 const Vector2 &t0 = Vector2(0.0f, 0.0f), const Vector2 &t1 = Vector2(1.0f, 1.0f));
    static void RenderScreenAlignedQuadRotate(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                 const Vector2 &t0 = Vector2(0.0f, 0.0f), const Vector2 &t1 = Vector2(1.0f, 1.0f), float angle = 0.f);
    static void RenderScreenAlignedPanoramaRotate(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end, float angle = 0.f);
    static void RenderScreenAlignedQuadDeferred(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end,
                                 const Vector2 &t0 = Vector2(0.0f, 0.0f), const Vector2 &t1 = Vector2(1.0f, 1.0f));
    static void RenderDeferred();
    static void Render9Slice(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end, float cornerSize, float middleRange);


    void RenderBackground(DeviceTexture *texture, float alpha);
    void RenderTitle(DeviceTexture *texture, float alpha, float scale, float z = -100.0f);
    void RenderOverlay(DeviceTexture *texture, const Vector4 &color, float scale = 1.0f);

    void RenderQuads();

    void Set(const char *name, ScreenQuadToRender &quad);
    void Remove(const char *name)
    {
        quads.erase(name);
    }

//  void SetRenderContext(const RenderContext &context) { this->context = context; }
    
private:

    static void RenderScreenAligned(DeviceTexture *texture, const Vector4 &color, const float *vertices, const float *uvs, int count);
    
//  RenderContext                             context;
    std::map<std::string, ScreenQuadToRender> quads;
};

#endif // _QUADRENDERER_H