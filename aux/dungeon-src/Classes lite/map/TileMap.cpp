/*
 *  TileMap_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 */

#include "TileMap.h"

#include "core/strs.h"
#include "events/Event.h"
#include "render/Renderable.h"
#include "render/OverheadCamera.h"

class LocalGameServices;
class OverheadCamera;

#include <map> // $DEBUG

SimpleTileInstance::SimpleTileInstance(ITile *tile) : tile(tile), owner(false), instance(0)
{
    if (!tile)
        return;
    IRenderable *r = tile->GetRenderableObject();
    if (!r)
        return;
    instance = r->CopyIfNecessary();
    if (r != instance)
        owner = true;
}

SimpleTileInstance::~SimpleTileInstance()
{
    if (owner)
        delete instance;
    instance = 0;    
}

ITile *SimpleTileInstance::GetTile()
{
    return tile;
}

void SimpleTileInstance::Update(const GameTime &time)
{
    if (instance)
        instance->Update(time);
}

IRenderable *SimpleTileInstance::GetRenderableObject()
{
    return instance;
}

RenderableTile::RenderableTile(Renderable *renderable, int id) : renderable(renderable), id(id)
{
}

RenderableTile::~RenderableTile()
{
    // $todo?
}

IRenderable *RenderableTile::GetRenderableObject()
{
    return renderable;
}

int RenderableTile::GetId() const
{
    return id;
}

ITileInstance *RenderableTile::CreateTileInstance()
{
    return new SimpleTileInstance(this);
}

void RenderableTile::Update(const GameTime &time)
{
    renderable->Update(time);
}

RenderableTileSet::RenderableTileSet(std::vector<Renderable *> objects, const core::Size &size) : size(size)
{
    for (size_t i = 0; i < objects.size(); i++)
        tiles.push_back(new RenderableTile(objects[i], (int)i));
}

RenderableTileSet::~RenderableTileSet()
{
    for (std::vector<ITile *>::iterator i = tiles.begin(), e = tiles.end(); i != e; ++i)
        delete (*i);
    tiles.clear();
}

int RenderableTileSet::GetCount() const
{
    return (int)tiles.size();
}

core::Size RenderableTileSet::GetTileSize() const
{
    return size;
}

ITile *RenderableTileSet::GetTile(int index)
{
    return tiles[index < 0 ? 0 : (index % GetCount())];
}

ITileMap *TileMaps::CreateMap(const core::Size &size)
{
    return new CoreTileMap(size, 0);
}

void TileMaps::Copy(ITileMap *src, ITileMap *dst)
{
    core::Size ssz = src->GetSizeInTiles(),
    dsz = dst->GetSizeInTiles();
    Copy(src, core::Rectangle(0, 0, ssz.width, ssz.height),
         dst, core::Rectangle(0, 0, dsz.width, dsz.height));
}

core::Point TileMaps::GetTileLocationAtWorld(const core::Size &tileSize, const core::Point &at)
{
    return core::Point(at.x / tileSize.width, at.y / tileSize.height);
}

core::Point TileMaps::GetTileLocationAtWorld(ITileMapModel *model, const core::Point &at)
{
    return GetTileLocationAtWorld(model->GetTileSize(), at);
}

int TileMaps::GetTileIdAtWorld(ITileMapModel *model, const core::Point &at)
{
    core::Point p     = GetTileLocationAtWorld(model->GetTileSize(), at);
    core::Size  mapsz = model->GetMapSizeInTiles();
    if (p.x < 0 || p.y < 0 || p.x >= mapsz.width || p.y >= mapsz.height)
        return -1;
    return model->GetTileId(p);
}

ITileInstance *TileMaps::GetTileInstanceAtWorld(ITileMapModel *model, const core::Point &at)
{
    core::Point p     = GetTileLocationAtWorld(model->GetTileSize(), at);
    core::Size  mapsz = model->GetMapSizeInTiles();
    if (p.x < 0 || p.y < 0 || p.x >= mapsz.width || p.y >= mapsz.height)
        return 0;
    return model->GetTileInstance(p);
}

void TileMaps::Copy(ITileMap *src, const core::Rectangle &from, ITileMap *dst, const core::Rectangle &to)
{
    core::Rectangle f(from), t(to);
    if (f.x < 0) f.x = 0;
    if (f.y < 0) f.y = 0;
    if (t.x < 0) t.x   = 0;
    if (t.y < 0) t.y   = 0;
    core::Size ssz   = src->GetSizeInTiles(),
    dsz   = dst->GetSizeInTiles(),
    sz    = core::Size(math::min(math::min(ssz.width  - f.x, f.width),    math::min(dsz.width  - t.x, t.width)),
                       math::min(math::min(ssz.height - f.y, f.height), math::min(dsz.height - t.y, t.height)));
    CopyRaw(src, core::Point(f.x, f.y), dst, core::Point(t.x, t.y), sz);
}

ITileMap *TileMaps::Copy(ITileMap *src, const core::Rectangle &from)
{
    ITileMap *map = new CoreTileMap(core::Size(from.width, from.height));
    Copy(src, from, map, core::Rectangle(0, 0, from.width, from.height));
    return map;
}

void TileMaps::CopyRaw(ITileMap *src, const core::Point &from, ITileMap *dst, const core::Point &to, const core::Size &sz)
{
    for (int y = 0; y < sz.height; y++)
        for (int x = 0; x < sz.width; x++)
            dst->SetTileId(to.x + x, to.y + y, src->GetTileId(from.x + x, from.y + y));
}

bool TileMaps::FindClosestTile(ITileMapModel *model, const core::Point &start, core::Point &point, TilePredicateCallback f, void *context)
{
    int         distancesq = 0x7fffffff;
    bool        found      = false;
    core::Size  sz         = model->GetMapSizeInTiles();
    
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
        {
            core::Point p(i, j);
            int dx  = start.x - i,
            dy  = start.y - j,
            dsq = dx * dx + dy * dy;                
            if (dsq < distancesq && f(model, p, context))
            {
                distancesq = dsq;
                point      = p;
                found      = true;
            }                
        }
    }
    return found;
}

core::Point TileMaps::FindEvent(ITileMapModel *map, const char *name)
{
    core::Size sz = map->GetMapSizeInTiles();
    for (int y = 0; y < sz.height; y++)
    {
        for (int x = 0; x < sz.width; x++)
        {
            ITileInstance *tile = map->GetTileInstance(x, y);
            Renderable    *r    = (Renderable *)tile->GetRenderableObject();
            if (!r)
                continue;
            std::vector<Event *> &events = r->GetEvents();
            if (!events.empty())
                for (std::vector<Event *>::const_iterator i = events.begin(), e = events.end(); i != e; ++i)
                    if (starts((*i)->text.c_str(), name))
                        return core::Point(x, y);
        }
    }
    return core::Point(-1, -1);
}

CoreTileMap::CoreTileMap(const core::Size &size) : size(size)
{
    Create(size, 0);
}

CoreTileMap::CoreTileMap(const core::Size &size, int initialValue) : size(size)
{
    Create(size, initialValue);
}

CoreTileMap::~CoreTileMap()
{
    delete [] data;
    data = 0;
}

void CoreTileMap::Reverse()
{
    int area = size.width * size.height;
    for (int i = 0, sz = area / 2; i < sz; i++)
    {
        int e = area - i - 1, t = 0;
        t = data[i];
        data[i] = data[e];
        data[e] = t;
    }
}

int CoreTileMap::GetTileId(int x, int y) const
{
    if (x < 0 || y < 0 || x >= size.width || y >= size.height)
        return 0;
    return data[y * size.width + x];
}

bool CoreTileMap::SetTileId(int x, int y, int id)
{
    if (x >= size.width || y >= size.height || id < 0)
        return false;
    data[y * size.width + x] = id;
    return true;
}

core::Size CoreTileMap::GetSizeInTiles() const
{
    return size;
}

void CoreTileMap::Create(const core::Size &size, int value)
{
    this->size = size;
    int area = size.width * size.height;
    data = new int [area];
    for (int i = 0; i < area; i++)
        data[i] = value;
}

TileInstanceMap::TileInstanceMap(const core::Size &size) : size(size)
{
    Create(size);
}


TileInstanceMap::~TileInstanceMap()
{
    for (int i = 0, sz = size.width * size.height; i < sz; i++)
        if (data[i]) delete data[i];
    delete [] data;
    data = 0;
}

ITileInstance *TileInstanceMap::GetTileInstance(int x, int y)
{
    if (x < 0 || y < 0 || x >= size.width || y >= size.height)
        return 0;
    return data[y * size.width + x];
}

bool TileInstanceMap::SetTileInstance(int x, int y, ITileInstance *instance)
{
    if (x >= size.width || y >= size.height)
        return false;
    data[y * size.width + x] = instance;
    return true;
}

core::Size TileInstanceMap::GetSizeInTiles() const
{
    return size;
}

void TileInstanceMap::Create(const core::Size &size)
{
    int area = size.width * size.height;
    data = new ITileInstance * [area];
    memset(data, 0, sizeof(ITileInstance *) * area);
}

SimpleTileMapModel::SimpleTileMapModel(ITileSet *set, ITileMap *map) : set(set), map(map)
{
    CreateInstanceMap();
}

SimpleTileMapModel::~SimpleTileMapModel()
{
    delete map;    
//    delete set; // do we delete this?
    delete instances;
}

ITileSet *SimpleTileMapModel::GetTileSet()
{
    return set;
}

ITileMap *SimpleTileMapModel::GetTileMap()
{
    return map;
}

ITile *SimpleTileMapModel::GetTile(int x, int y)
{
    return set->GetTile(map->GetTileId(x, y));
}

ITile *SimpleTileMapModel::GetTile(const core::Point &p)
{
    return GetTile(p.x, p.y);
}

ITile *SimpleTileMapModel::GetTile(int index)
{
    return set->GetTile(index);
}

ITileInstance *SimpleTileMapModel::GetTileInstance(int x, int y)
{
    return instances->GetTileInstance(x, y);
}

ITileInstance *SimpleTileMapModel::GetTileInstance(const core::Point &p)
{
    return GetTileInstance(p.x, p.y);
}

int SimpleTileMapModel::GetTileId(int x, int y) const
{
    return map->GetTileId(x, y);
}

bool SimpleTileMapModel::SetTileId(int x, int y, int index)
{
    if (index >= set->GetCount())
        return false;
    if (index == map->GetTileId(x, y))
        return true;
    bool r = map->SetTileId(x, y, index);
    if (r) UpdateInstance(x, y);
    return r;
}

int SimpleTileMapModel::GetTileId(const core::Point &p) const
{
    return GetTileId(p.x, p.y);
}

bool SimpleTileMapModel::SetTileId(const core::Point &p, int id)
{
    return SetTileId(p.x, p.y, id);
}

core::Size SimpleTileMapModel::GetTileSize() const
{
    return set->GetTileSize();
}

core::Size SimpleTileMapModel::GetMapSizeInTiles() const
{
    return map->GetSizeInTiles();
}

core::Size SimpleTileMapModel::GetMapSizeInPixels() const
{
    core::Size mapsz  = GetMapSizeInTiles(),
    tilesz = GetTileSize();
    return core::Size(mapsz.width * tilesz.width, mapsz.height * tilesz.height);
}

void SimpleTileMapModel::Update(const GameTime &time)
{
    core::Size size = GetMapSizeInTiles();
    // we probably want to window this somehow..
    for (int j = 0; j < size.height; j++)
    {
        for (int i = 0; i < size.width; i++)
        {
            ITileInstance *tile = GetTileInstance(i, j);
            if (tile != 0)
                tile->Update(time);
        }
    }
}

IRenderable *SimpleTileMapModel::GetRenderableObject(int x, int y)
{
    ITileInstance *tile = GetTileInstance(x, y);
    return tile ? tile->GetRenderableObject() : 0;
}

void SimpleTileMapModel::CreateInstanceMap()
{
    core::Size sz = map->GetSizeInTiles();    
    instances = new TileInstanceMap(sz);
    for (int j = 0; j < sz.height; j++)
        for (int i = 0; i < sz.width; i++)
            UpdateInstance(i, j);
}

void SimpleTileMapModel::UpdateInstance(int x, int y)
{
    ITile *tile = GetTile(x, y);
    if (tile != 0)
        instances->SetTileInstance(x, y, tile->CreateTileInstance());
}

static void RenderTile(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera, bool clip, int x, int y)
{
    static Matrix mp;    
    core::Size   tilesz = model->GetTileSize();//, mapsz  = model->GetMapSizeInTiles();
    IRenderable *r      = model->GetRenderableObject(x, y);
    if (r)
    {
        Vector3 pos(at.x + x * tilesz.width, at.y + y * tilesz.height, at.z);
        mp.translate(pos);
        r->Render(device, mp, camera, false);
    }    
}

static Vector3 GetTranslation(ITileMapModel *model, const Vector3 &at, int x, int y)
{
    core::Size tsz = model->GetTileSize();
    return Vector3(at.x + x * tsz.width, at.y + y * tsz.height, at.z);
}

std::map<ITileMapModel *, core::Size> _debug_last;

static void RenderRange(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera, const core::Rectangle *area_)
{
    core::Size mapsz = model->GetMapSizeInTiles();
    core::Rectangle area(0, 0, mapsz.width, mapsz.height);
    if (area_) area = *area_;
    for (int j = area.y, jsz = area.y + area.height; j < jsz; j++)
        for (int i = area.x, isz = area.x + area.width; i < isz; i++)
            RenderTile(device, model, at, camera, false, i, j);
    /*
core::Size last = _debug_last[model];
if (last.width != area.width || last.height != area.height)
{
printf("Render Range (%p): (%d, %d) [%u, %u]\n", model, area.x, area.y, area.width, area.height);
last.width = area.width; last.height = area.height;
_debug_last[model] = last;
}
     */
}

core::Point GetTileLocationFrom(const core::Size &tilesz, const Vector3 &at, const Vector3 &position)
{
    return core::Point(int((position.x - at.x) / tilesz.width), 
                       int((position.y - at.y) / tilesz.height));
}
/*
core::Rectangle FindRange(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera)
{
    AABox bounds = TileMapRenderer::GetVisibleMapRegion(model, at, camera);
    core::Point a(GetTileLocationFrom(model->GetTileSize(), at, bounds.minimum)),
                b(GetTileLocationFrom(model->GetTileSize(), at, bounds.maximum));
    core::Rectangle r(0, 0, 0, 0);
    
    if (a.x < b.x) { r.x = a.x; r.width  = b.x - a.x; }
    else           { r.x = b.x; r.width  = a.x - b.x; }
    if (a.y < b.y) { r.y = a.y;    r.height = b.y - a.y; }
    else           { r.y = b.y; r.height = a.y - b.y; }    
    r.width++; r.height++;
    return r;
}
*/
core::Rectangle FindRange(ITileMapModel *model, const Vector3 &at, const AABox &bbounds, const OverheadCamera &camera)
{
    AABox bounds = TileMapRenderer::GetVisibleMapRegion(model, bbounds, camera);
    core::Size  mapsz = model->GetMapSizeInTiles(), tilesz = model->GetTileSize();
    core::Point a(GetTileLocationFrom(tilesz, at, bounds.minimum/* - Vector3(tilesz.width * 0.5f, tilesz.height * 0.5f)*/)),
                b(GetTileLocationFrom(tilesz, at, bounds.maximum/* + Vector3(tilesz.width * 0.5f, tilesz.height * 0.5f)*/));
    core::Rectangle r(core::Rectangle::CreateFromPoints(a, b));
    
    bool print = false;
    if (r.x < 0 || r.y < 0)
    {
    //    printf("rect: (%4d, %4d) [%4u, %4u] ->", r.x, r.y, r.width, r.height);
        print = true;
    }
    r = core::Rectangle::Overlap(r, core::Rectangle(0, 0, mapsz));
//    if (print)
//        printf("(%4d, %4d) [%4u, %4u]\n", r.x, r.y, r.width, r.height);
//    if (!r.width || !r.height)
//        r.width = r.height = 0;
// $we need to do this the right way..
    /*
    enum { EXPAND = 4 };
r.x      -= EXPAND;
r.y      -= EXPAND;
r.width  += EXPAND * 2;
r.height += EXPAND * 2;
     */
    r.width += 2; r.height += 2;
    return r;
}

AABox GetCornerBounds(ITileMapModel *model, const Vector3 &at, int x, int y)
{
    core::Size mapsz = model->GetMapSizeInTiles();
    Vector3    pos(GetTranslation(model, at, x, y));
    IRenderable *r = model->GetRenderableObject(x, y);
    AABox box;
    if (r)
    {
        box.insert(r->GetBounds());
        Matrix mt;
        mt.translate(pos);
        box.minimum = mt * box.minimum;
        box.maximum = mt * box.maximum;
    }
    return box;
}

AABox GetMapBounds(ITileMapModel *model, const Vector3 &at, const core::Rectangle &area)
{
    AABox box;
    box.insert(GetCornerBounds(model, at, area.x,         area.y));
    box.insert(GetCornerBounds(model, at, area.x,         area.height - 1));    
    box.insert(GetCornerBounds(model, at, area.width - 1, area.y));    
    box.insert(GetCornerBounds(model, at, area.width - 1, area.height - 1));
    return box;
}

bool IsAnyOfMapAreaVisible(ITileMapModel *model, const core::Rectangle &area, const Vector3 &at, const OverheadCamera &camera)
{
    static const AABox screen(Vector3(-1.0f, -1.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f));
    
    AABox   bounds = GetMapBounds(model, at, area);
    Matrix  vp(camera.GetViewProjection());
    AABox   map(vp * bounds.minimum, vp * bounds.maximum);
    map.minimum /= map.minimum.z; map.maximum /= map.maximum.z;

    AABox overlap(screen);
    overlap.insert(map);
    
    Vector3 osz(overlap.size()), msz(map.size()), ssz(screen.size());    
    return (osz.x < (msz.x + ssz.x)) && (osz.y < (msz.y + ssz.y));
}

bool IsAnyOfMapAreaVisible(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera)
{
    core::Size msz = model->GetMapSizeInTiles();    
    core::Rectangle area(0, 0, msz.width, msz.height);
    return IsAnyOfMapAreaVisible(model, area, at, camera);
}
/*
void TileMapRenderer::Render(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera)
{
//    if (IsAnyOfMapAreaVisible(model, at, camera))
    {
        core::Rectangle range = FindRange(model, at, camera);
        RenderRange(device, model, at, camera, &range);
    }
}
*/
void TileMapRenderer::Render(GraphicsDevice &device, ITileMapModel *model, const Vector3 &at, const AABox &bounds, const OverheadCamera &camera)
{
    //    if (IsAnyOfMapAreaVisible(model, at, camera))
    {
        core::Rectangle range = FindRange(model, at, bounds, camera);
        RenderRange(device, model, at, camera, &range);
    }
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
                IRenderable *r = tile->GetRenderableObject();
                if (r)
                {
                    t.translate(GetTranslation(model, Vector3::zero(), i, j));
                    bounds.insert(transform(r->GetBounds(), t));
                }
            }
        }
    }
/*
static bool printed = false;
    bounds = AABox(at - Vector3(tsz.width * 0.5f, tsz.height * 0.5f, 0.0f), at + Vector3(msz.width * tsz.width, msz.height * tsz.height, 0.0f) - Vector3(tsz.width * 0.5f, tsz.height * 0.5f, 0.0f));
    if (!printed)
    {
        printf("bounds: %u, %u | %.2f %.2f - %.2f %.2f\n", msz.width, msz.height, bounds.minimum.x, bounds.minimum.y, bounds.maximum.x, bounds.maximum.y);
        printed = true;
    }
 */
    return bounds;
}

/*
AABox TileMapRenderer::GetVisibleMapRegion(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera)
{
    core::Size sz = model->GetMapSizeInTiles();
    AABox visible, bounds = GetMapBounds(model, at, core::Rectangle(0, 0, sz.width, sz.height));
    Vector4 plane = MathUtil::CreatePlaneFromVertices(Vector3(bounds.minimum.x, bounds.minimum.y, bounds.maximum.z),
                                                      Vector3(bounds.maximum.x, bounds.minimum.y, bounds.maximum.z),
                                                      Vector3(bounds.minimum.x, bounds.maximum.y, bounds.maximum.z));
    VisibleBounds vb(plane, camera);
    for (int i = 0; i < 4; i++)
        if (vb.valid[i])
            visible.insert(vb.at[i]);
    bounds.overlap(visible);
    return bounds;
}
*/
/*
AABox TileMapRenderer::GetVisibleMapRegion(ITileMapModel *model, const Vector3 &at, const OverheadCamera &camera)
{
    core::Size sz = model->GetMapSizeInTiles();    
    AABox bounds = GetMapBounds(model, at, core::Rectangle(0, 0, sz.width, sz.height));
    return GetVisibleMapRegion(model, bounds, camera);
}
*/

extern void _DebugRenderBoundsQuad(const AABox &box);

AABox TileMapRenderer::GetVisibleMapRegion(ITileMapModel *model, const AABox &bounds, const OverheadCamera &camera)
{
    core::Size sz = model->GetMapSizeInTiles();
    AABox visible;
    float z = bounds.maximum.z + 128.0f;
//    printf("bounds z = %.2f (minz %.2f)\n", bounds.maximum.z, bounds.minimum.z);
    Vector4 plane = MathUtil::CreatePlaneFromVertices(Vector3(bounds.minimum.x, bounds.minimum.y, z),
                                                      Vector3(bounds.maximum.x, bounds.minimum.y, z),
                                                      Vector3(bounds.minimum.x, bounds.maximum.y, z));
    VisibleBounds vb(plane, camera);
    for (int i = 0; i < 4; i++)
        if (vb.valid[i])
            visible.insert(vb.at[i]);
    //visible = bounds.overlap(visible);
//    _DebugRenderBoundsQuad(visible);
/*
    _DebugRenderBoundsQuad(AABox(visible.minimum - Vector3(25.0f, 25.0f, 0.0f), 
                                 visible.minimum + Vector3(25.0f, 25.0f, 0.0f)));
    _DebugRenderBoundsQuad(AABox(visible.maximum - Vector3(25.0f, 25.0f, 0.0f),
                                 visible.maximum + Vector3(25.0f, 25.0f, 0.0f)));
    _DebugRenderBoundsQuad(AABox(Vector3(visible.maximum.x, visible.minimum.y, visible.maximum.z) - Vector3(25.0f, 25.0f, 0.0f),
                                 Vector3(visible.maximum.x, visible.minimum.y, visible.maximum.z) + Vector3(25.0f, 25.0f, 0.0f)));
    _DebugRenderBoundsQuad(AABox(Vector3(visible.minimum.x, visible.maximum.y, visible.maximum.z) - Vector3(25.0f, 25.0f, 0.0f),
                                 Vector3(visible.minimum.x, visible.maximum.y, visible.maximum.z) + Vector3(25.0f, 25.0f, 0.0f)));    
*/
//    vb.Print();
    /*
    for (int i = 0; i < 4; i++)
    {
        if (vb.valid[i])
        {
            AABox box(vb.at[i], vb.at[i]);
            box.expand(Vector3(50.0f, 50.0f, 0.0f));
            _DebugRenderBoundsQuad(box);
        }
    }
    */
 
    return visible;
}
