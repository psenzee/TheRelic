#pragma once

#include "core/core.h"
#include "render/Animator.h"
#include "fast/Allocator.h"

class DeviceTexture;
class OverheadCamera;
class RenderContext;
class XmlElement;

class GlyphExtent
{
public:
    
    float   width, offset;
    Vector2 uv0, uv1;
    
    GlyphExtent() : width(1.0f), offset(0.0f) {}
};

class Glyph
{
public:

    GlyphExtent extent;
    Vector2     position;
    Vector2     scale;
    unsigned    color;
        
    // the prototypical quad in worldspace will be -0.5 to 0.5
    Glyph(const GlyphExtent &extent, const Vector2 &position, const Vector2 &scale, uint32_t color)
        : extent(extent), position(position), scale(scale), color(color) {}
};
