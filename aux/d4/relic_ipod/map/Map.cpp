#include "Map.h"

#include "core/core.h"
#include "core/simplexml.h"
#include "map/TileMap.h"
#include "gamecore/IGameObject.h"
#include "gamecore/ICollidable.h"
#include "render/RenderContext.h"
#include "render/OverheadCamera.h"
#include "render/GraphicsDevice.h"
#include "TileMapRenderer.h"

bool MapCollider(const Vector3 &p, float radius, Vector3 &resolve, void *user)
{
    if (!user)
        return 0;
    Map *map = reinterpret_cast<Map *>(user);
    ICollidable::Classification c = map->Collision(p, radius, resolve);
    return c == ICollidable::CLASS_ON || c == ICollidable::CLASS_IN;
}

inline static AABox transform(const AABox &box, const Matrix &transform)
{
    return AABox(transform * box.minimum, transform * box.maximum);
}

Map::Map(ITileMapModel *model) : MapModel(model), Start(0, 0), End(0, 0)
{        
    Start = FindEvent("Start");
    End   = FindEvent("ExitLevel");
    core::Size mapsz  = MapModel->GetMapSizeInTiles(),
               tilesz = MapModel->GetTileSize();
/*
    for (int j = 0; j < mapsz.height; j++)
    {
        for (int i = 0; i < mapsz.width; i++)
        {
            core::Point p(i, j);
            ITileInstance *tile = model->GetTileInstance(p);
            if (tile)
            {
                IGameObject *r = tile->GetGameObject();
                if (r && r->IsMutable())
                    dynamic.push_back(p);
                // $TODO DO WE REALLY NEED THIS FLOOR STUFF?
//                if (IsFloorTile(model->GetTileId(p)))
//                    floors.push_back(p);
            }
        }
    }
*/
    basebounds = TileMapRenderer::GetMapBoundsSlow(model, Vector3(0.0f, 0.0f, 0.0f), core::Rectangle(0, 0, mapsz.width, mapsz.height));
    bounds = basebounds;
}

Map::~Map()
{
    delete MapModel;
    MapModel = 0;
}
            
void Map::SetPlayerTile(const Vector2 &position, int id)
{
    core::Point p = TileMaps::GetTileLocationAtWorld(MapModel,
                                                    core::Point((int)(position.x - Position.x), (int)(position.y - Position.y)));
    MapModel->SetTileId(p, id);
}
            
void Map::Update(RenderContext &context, const GameTime &time)
{
    /*
    for (std::vector<core::Point>::iterator i = dynamic.begin(), e = dynamic.end(); i != e; ++i)
    {
        ITileInstance *tile = MapModel->GetTileInstance(*i);
        if (tile)
        {
            IGameObject *r = tile->GetGameObject();
            if (r)
                r->Update(time);    
        }
    }
    */
    mRenderRange = TileMapRenderer::FindRange(MapModel, Position, GetBounds(), context.camera);
}
            
core::Point Map::FindEvent(const char *name) const
{
    return TileMaps::FindEvent(MapModel, name);
}
            
Vector3 Map::GetLookAt(const Vector2 &player) const
{
    return Vector3(player.x, player.y, 0.0f);
}

void Map::SetPosition(const Vector3 &position)
{ 
    Position = position;
    bounds = AABox(basebounds.minimum + position, basebounds.maximum + position);
}

void Map::MovePosition(const Vector3 &offset)
{
    SetPosition(GetPosition() + offset);
}

Vector3 Map::GetEndPoint() const
{
    core::Point p(End);
    core::Size  sz = MapModel->GetTileSize();
    return Vector3(float(p.x * sz.width), float(p.y * sz.height), 0.0f);
}

void Map::SetEndTile(int id)
{
    MapModel->SetTileId(End, id);
}

Vector3 Map::GetPointInWorld(const core::Point &p) const
{
    core::Size  sz = MapModel->GetTileSize();
    return Vector3(float(p.x * sz.width), float(p.y * sz.height), 0.0f) + Position;    
}

Vector3 Map::GetEventPointInWorld(const char *event) const
{
    return GetPointInWorld(FindEvent(event));
}

Vector3 Map::GetStartPoint() const
{
    core::Point p(Start);
    core::Size  sz = MapModel->GetTileSize();
    return Vector3(float(p.x * sz.width), float(p.y * sz.height), 0.0f);
}

Vector3 Map::GetSize() const
{
    core::Size sz = MapModel->GetMapSizeInPixels();
    return Vector3((float)sz.width, (float)sz.height, 0.0f);
}
            
void Map::Draw(RenderContext &context, const GameTime &time)
{
    core::Size framesz = context.device.GetFrameSize(),
               tilesz  = MapModel->GetTileSize();
    TileMapRenderer::RenderRange(context, MapModel, Position, &mRenderRange);
}

ITile *Map::GetTileAt(const Vector2 &p)
{
    return TileMaps::GetTileAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)));
}

int Map::GetTileIdAt(const Vector2 &p) const
{
    return TileMaps::GetTileIdAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)));    
}

void Map::SetTileIdAt(const Vector2 &p, int id)
{
    return TileMaps::SetTileIdAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)), id);    
}

core::Point Map::GetTileLocationAt(const Vector2 &p) const
{
    return TileMaps::GetTileLocationAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)));    
}
            
Vector3 Map::GetNormalizedPosition(const Vector3 &at) const
{
    return at - Position;
}

ICollidable::Classification Map::Collision(const Vector3 &p, float radius, Vector3 &resolve)
{
	core::Size sz = GetMapModel()->GetTileSize();
    Vector3 halfSize(sz.width * 0.5f, sz.height * 0.5f, 0.f);
    Vector3 q(p + halfSize);
    core::Point r     = core::Point(int(q.x / sz.width), int(q.y / sz.height));
    core::Size  mapsz = MapModel->GetMapSizeInTiles();
    if (r.x < 0 || r.y < 0 || r.x >= mapsz.width || r.y >= mapsz.height)
        return ICollidable::CLASS_IN;
    ITile *tile = MapModel->GetTile(r);

    if (!tile)
        return ICollidable::CLASS_IN; // beyond the boundaries..
    IGameObject *object = tile->GetGameObject();
    if (!object)
        return ICollidable::CLASS_OUT; // no geometry associated
    ICollidable *collider = object->GetCollidable();
    if (!collider)
        return ICollidable::CLASS_OUT; // no collider associated
    Vector3 pointAt(q.x - r.x * sz.width, q.y - r.y * sz.height, q.z);
    return collider->ClassifySphere(pointAt - halfSize, radius, resolve);
}