/*
 *  TileMap.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TileMapRenderer.h"

#include "core/strs.h"
#include "events/Event.h"
#include "render/OverheadCamera.h"
#include "render/RenderContext.h"
#include "gamecore/IGameObject.h"
#include "gamecore/IRenderable.h"

static void RenderTile(RenderContext &context, ITileMapModel *model, const Vector3 &at, bool clip, int x, int y)
{
    static Matrix mp;
    core::Size   tilesz = model->GetTileSize();
    IGameObject *r      = model->GetGameObject(x, y);
    if (r)
    {
        Vector3 pos(at.x + x * tilesz.width, at.y + y * tilesz.height, at.z);
        
        mp.translate(pos);
        //r->Render(context, mp, false);
        
        RenderContext rc(context);
       // rc.transform = context.transform * mp;
        rc.transform = mp;// * context.transform;
        r->GetRenderable().Render(rc);
    }    
}

static Vector3 GetTranslation(ITileMapModel *model, const Vector3 &at, int x, int y)
{
    core::Size tsz = model->GetTileSize();
    return Vector3(at.x + x * tsz.width, at.y + y * tsz.height, at.z);
}

void TileMapRenderer::RenderRange(RenderContext &context, ITileMapModel *model, const Vector3 &at, const core::Rectangle *area_)
{
    core::Size mapsz = model->GetMapSizeInTiles();
    core::Rectangle area(0, 0, mapsz.width, mapsz.height);
    if (area_) area = *area_;
    for (int j = area.y, jsz = area.y + area.height; j < jsz; j++)
        for (int i = area.x, isz = area.x + area.width; i < isz; i++)
            RenderTile(context, model, at, false, i, j);
}

core::Point GetTileLocationFrom(const core::Size &tilesz, const Vector3 &at, const Vector3 &position)
{
    return core::Point(int((position.x - at.x) / tilesz.width), 
                       int((position.y - at.y) / tilesz.height));
}

core::Rectangle TileMapRenderer::FindRange(ITileMapModel *model, const Vector3 &at, const AABox &bbounds, const OverheadCamera &camera)
{
    AABox bounds = TileMapRenderer::GetVisibleMapRegion(model, bbounds, camera);
    core::Size  mapsz = model->GetMapSizeInTiles(), tilesz = model->GetTileSize();
    core::Point a(GetTileLocationFrom(tilesz, at, bounds.minimum/* - Vector3(tilesz.width * 0.5f, tilesz.height * 0.5f)*/)),
                b(GetTileLocationFrom(tilesz, at, bounds.maximum/* + Vector3(tilesz.width * 0.5f, tilesz.height * 0.5f)*/));
    core::Rectangle r(core::Rectangle::CreateFromPoints(a, b));    
    r = core::Rectangle::Overlap(r, core::Rectangle(0, 0, mapsz));
    r.width += 2; r.height += 2; // $TODO WHY 2, 2 ?
    return r;
}

void TileMapRenderer::Render(RenderContext &context, ITileMapModel *model, const Vector3 &at, const AABox &bounds)
{
    core::Rectangle range = FindRange(model, at, bounds, context.camera);
    RenderRange(context, model, at, &range);
}

inline static AABox transform(const AABox &box, const Matrix &transform)
{
    return AABox(transform * box.minimum, transform * box.maximum);
}

AABox TileMapRenderer::GetMapBoundsSlow(ITileMapModel *model, const Vector3 &at, const core::Rectangle &area)
{
    core::Size msz = model->GetMapSizeInTiles(), tsz = model->GetTileSize();
    AABox bounds;

    Matrix t;
    for (int j = area.y; j < area.height; j++)
    {
        for (int i = area.x; i < area.width; i++)
        {
            core::Point p(i, j);
            ITileInstance *tile = model->GetTileInstance(p);
            if (tile)
            {
                IGameObject *r = tile->GetGameObject();
                if (r)
                {
                    t.translate(GetTranslation(model, Vector3::zero(), i, j));
                    bounds.insert(transform(r->GetRenderable().GetBounds(), t));
                }
            }
        }
    }
    return bounds;
}

extern void _DebugRenderBoundsQuad(const AABox &box);

AABox TileMapRenderer::GetVisibleMapRegion(ITileMapModel *model, const AABox &bounds, const OverheadCamera &camera)
{
    // $TODO AT SOME POINT, WE'LL WANT TO MAKE THIS SUPPORT NON-AXIS ALIGNED VIEWS
    core::Size sz = model->GetMapSizeInTiles();
    AABox visible;
    float z = bounds.maximum.z; // here we want the z furthest away from the camera..
    Vector4 plane = MathUtil::CreatePlaneFromVertices(Vector3(bounds.minimum.x, bounds.minimum.y, z),
                                                      Vector3(bounds.maximum.x, bounds.minimum.y, z),
                                                      Vector3(bounds.minimum.x, bounds.maximum.y, z));
    VisibleBounds vb(plane, camera);
    for (int i = 0; i < 4; i++)
        if (vb.valid[i])
            visible.insert(vb.at[i]);
    return visible;
}
