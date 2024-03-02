#ifndef _TILEMAPRENDERER_H
#define _TILEMAPRENDERER_H

#include "core/core.h"
#include "core/aabox.h"

#include "ITileMap.h"
#include "TileMap.h"

#include <vector>

class IGameObject;
class RenderContext;
class OverheadCamera;
class BitMap2d;

class TileMapRenderer
{
public:
    
    static void  Render(RenderContext &context, ITileMapModel *model, const Vector3 &at, bool renderDefaults);
    static void  Render(RenderContext &context, ITileMapModel *model, const Vector3 &at, const AABox &bounds, bool renderDefaults, const BitMap2d *mask);
    
    static AABox GetVisibleMapRegion(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera);
    static AABox GetVisibleMapRegion(ITileMapModel *model, const AABox &bounds, const OverheadCamera &camera);
    
    // not to be used every frame!
    static AABox GetMapBoundsSlow(ITileMapModel *model, const Vector3 &at, const core::Rectangle &area);

    static void            RenderRange(RenderContext &context, ITileMapModel *model, const Vector3 &at, const core::Rectangle *area, bool renderDefaults, const BitMap2d *mask);
    static core::Rectangle FindRange(ITileMapModel *model, const Vector3 &at, const AABox &bbounds, const OverheadCamera &camera);
};

#endif // _TILEMAPRENDERER_H