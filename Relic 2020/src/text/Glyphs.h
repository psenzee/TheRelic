#pragma once

#include "core/core.h"
#include "fast/Allocator.h"
#include "QuadListRenderer.h"
#include "CommonVertex.h"

class RenderContext;
class DeviceTexture;

class Glyphs
{
public:
    
    typedef QuadListRenderer<CommonVertex> renderer_t;

    int                     Render(RenderContext &context, const GameTime &time);
    
    renderer_t             &GetQuadRenderer() { return _renderer; }

    static void             CreateInstance()  { if (!_instance) { _instance = new Glyphs; } }
    static Glyphs          *GetInstance()     { return _instance; }
    static void             DestroyInstance() { if (_instance) delete _instance; _instance = 0; }

private:

    Glyphs() {}

    static Glyphs *_instance;

    renderer_t _renderer;
};

