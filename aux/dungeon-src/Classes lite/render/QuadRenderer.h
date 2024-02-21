/*
 *  QuadRenderer.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _QUADRENDERER_H
#define _QUADRENDERER_H

#include "core/xna.h"

class OverheadCamera;
class GraphicsDevice;
class DeviceTexture;

class QuadRenderer
{
public:
    
    inline QuadRenderer(const OverheadCamera &camera, GraphicsDevice &device) : camera(camera), device(device) {}

    void RenderScreenQuad(DeviceTexture *texture, const Vector4 &color, float width, float height, const Vector3 &s, const Vector3 &t,
                                                  const Vector2 &uvs = Vector2(1.0f, 1.0f), const Vector2 &uvt = Vector2(0.0f, 0.0f));
    void RenderScreenAlignedQuad(DeviceTexture *texture, const Vector4 &color, const Vector3 &start, const Vector3 &end);    
    void RenderBackground(DeviceTexture *texture, float alpha);
    void RenderTitle(DeviceTexture *texture, float alpha, float scale, float z = -100.0f);
    void RenderOverlay(DeviceTexture *texture, const Vector4 &color, float scale = 1.0f);
    
private:
    
    const OverheadCamera &camera;
    GraphicsDevice       &device;
};

#endif // _QUADRENDERER_H