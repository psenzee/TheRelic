/*
 *  Map.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Map.h"

#include "core/simplexml.h"
#include "core/xna.h"
#include "map/TileMap.h"
#include "map/Maze.h"
#include "render/LocalGameServices.h"
#include "render/OverheadCamera.h"
#include "render/IRenderable.h"
 
MapFile::MapFile(XmlElement *level, const char *name)
{
    /*
    XmlElement xml = XmlUtil.GetElementWithKeyEqualTo(level, "map", "name", name);
                
    this.name        = name;
    this.description = XmlUtil.GetAttribute(xml, "description");
    this.tiles       = XmlUtil.GetAttribute(xml, "tiles");
    this.file        = XmlUtil.GetAttribute(xml, "file");
     */
};

inline static AABox transform(const AABox &box, const Matrix &transform)
{
    return AABox(transform * box.minimum, transform * box.maximum);
}

Map::Map(LocalGameServices *services, ITileMapModel *model, MapFile *options) : MapModel(model), Options(options), Services(services), Start(0, 0), End(0, 0)
{        
    Start = FindEvent("Start");
    End   = FindEvent("ExitLevel");
    core::Size mapsz  = MapModel->GetMapSizeInTiles(),
               tilesz = MapModel->GetTileSize();
    for (int j = 0; j < mapsz.height; j++)
    {
        for (int i = 0; i < mapsz.width; i++)
        {
            core::Point p(i, j);
            ITileInstance *tile = model->GetTileInstance(p);
            if (tile)
            {
                IRenderable *r = tile->GetRenderableObject();
                if (r && !r->IsStatic())
                    dynamic.push_back(p);
                if (IsFloorTile(model->GetTileId(p)))
                    floors.push_back(p);
            }
        }
    }
    basebounds = TileMapRenderer::GetMapBoundsSlow(model, Vector3(0.0f, 0.0f, 0.0f), core::Rectangle(0, 0, mapsz.width, mapsz.height));
    bounds = basebounds;
}

Map::~Map()
{
    delete MapModel;
    MapModel = 0;
    Options = 0;    
}
            
void Map::SetPlayerTile(const Vector2 &position, int id)
{
    core::Point p = TileMaps::GetTileLocationAtWorld(MapModel,
                                                    core::Point((int)(position.x - Position.x), (int)(position.y - Position.y)));
    MapModel->SetTileId(p, id);
}
            
void Map::ClearPickup(const Vector2 &position)
{
    core::Point p = TileMaps::GetTileLocationAtWorld(MapModel,
                                                    core::Point((int)position.x, (int)position.y));
    MapModel->SetTileId(p, (int)GetFloorTileAt(p));
}
            
bool Map::IsFloorTile(int id)
{
    return id == (int)FLOOR_TILE1 || id == (int)FLOOR_TILE2;
}
            
void Map::Update(const GameTime &time)
{
    for (std::vector<core::Point>::iterator i = dynamic.begin(), e = dynamic.end(); i != e; ++i)
    {
        ITileInstance *tile = MapModel->GetTileInstance(*i);
        if (tile)
        {
            IRenderable *r = tile->GetRenderableObject();
            if (r)
                r->Update(time);    
        }
    }
}
            
core::Point Map::FindEvent(const char *name) const
{
    return TileMaps::FindEvent(MapModel, name);
}
            
Vector3 Map::GetLookAt(const Vector2 &player) const
{
    return Vector3(player.x, player.y, 0.0f);
}

Vector3 Map::GetRandomFloorPosition(core::Random &r) const
{
    core::Point p = floors[r.integer() % floors.size()];
    core::Size sz = MapModel->GetTileSize();    
    return Vector3(float(p.x * sz.width), float(p.y * sz.height), 0.0f) + Position;
}

void ClearShortestRow(ITileMapModel *model, int y, int from)
{
    core::Size sz = model->GetMapSizeInTiles();
    if (from < sz.width / 2)
        for (int i = 0; i <= from; i++)
            model->SetTileId(core::Point(i, y), 0);
    else
        for (int i = from; i < sz.width; i++)
            model->SetTileId(core::Point(i, y), 0);    
}

void ClearSmallestBlock(ITileMapModel *model, const core::Point &p)
{
    core::Size sz = model->GetMapSizeInTiles();
    if (p.y < sz.width / 2)
        for (int i = 0; i <= p.y; i++)
            ClearShortestRow(model, i, p.x);
    else
        for (int i = p.y; i < sz.width; i++)
            ClearShortestRow(model, i, p.x);
}

void Map::ClearEndArea()
{
    ClearSmallestBlock(MapModel, End);
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
            
void Map::Draw(const GameTime &time, const OverheadCamera &camera)
{
    core::Size framesz = Services->frame,                
               tilesz  = MapModel->GetTileSize();
    TileMapRenderer::Render(*(Services->device), MapModel, Position, GetBounds(), camera);
}

int Map::GetTileIdAt(const Vector2 &p) const
{
    return TileMaps::GetTileIdAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)));    
}

core::Point Map::GetTileLocationAt(const Vector2 &p) const
{
    return TileMaps::GetTileLocationAtWorld(MapModel, core::Point(int(p.x - Position.x), int(p.y - Position.y)));    
}
        
Map::SpecialTile Map::GetFloorTileAt(const core::Point &at)
{
    return ((at.x + at.y) % 2) == 0 ? FLOOR_TILE1 : FLOOR_TILE2;
}
            
Vector3 Map::GetNormalizedPosition(const Vector3 &at) const
{
    return at - Position;
}

static bool OnEdge(ITileMapModel *map, const core::Point &p, int offset)
{
    if (p.x == offset || p.y == offset)
        return true;    
    core::Size mapsz = map->GetMapSizeInTiles();
    if (p.x == mapsz.width - 1 - offset || p.y == mapsz.height - 1 - offset)
        return true;
    return false;
}

enum {
       HORIZONTAL                  = MazeGenerator::HORIZONTAL,
       VERTICAL                    = MazeGenerator::VERTICAL,
       DAMAGED_HORIZONTAL_TRUSS    = 244,
       DAMAGED_VERTICAL_TRUSS      = 245,
       DAMAGED_CROSS_TRUSS         = 246,
       MISSING_HORIZONTAL_TRUSS    = 247,
       MISSING_VERTICAL_TRUSS      = 248,
       MISSING_CROSS_TRUSS         = 249,
       HORIZONTAL_BROKEN_0         = 251,
       HORIZONTAL_BROKEN_1         = 252,
       VERTICAL_BROKEN_0           = 261, 
       VERTICAL_BROKEN_1           = 262,
       DAMAGED_FLOOR_1             = 254,
       DAMAGED_FLOOR_2             = 255,
       DAMAGED_FLOOR_3             = 256,
       DAMAGED_FLOOR_4             = 257,        
       MISSING_FLOOR               = 258 };

static void SetDamagedTile(ITileMapModel *map, const core::Point &p)
{
    enum { DIVISOR = 4 };
    core::Size mapsz = map->GetMapSizeInTiles();
    int id = DAMAGED_FLOOR_1 + rand() % 4; // four types of damaged floor
    int modw = p.x % DIVISOR,
        modh = p.y % DIVISOR;
    if      (!modw && !modh) id = DAMAGED_CROSS_TRUSS;
    else if (!modw)          id = DAMAGED_VERTICAL_TRUSS;
    else if (!modh)          id = DAMAGED_HORIZONTAL_TRUSS;        
    map->SetTileId(p, id);
}

static void SetMissingTile(ITileMapModel *map, const core::Point &p)
{
    int id = MISSING_FLOOR, prev = map->GetTileId(p);    
    if (prev == HORIZONTAL_BROKEN_0 || prev == VERTICAL_BROKEN_0)
    {
        SetDamagedTile(map, p);
        prev = map->GetTileId(p);
    }
    if (OnEdge(map, p, 1))
        return;
    switch (prev)
    {
    case DAMAGED_HORIZONTAL_TRUSS: id = MISSING_HORIZONTAL_TRUSS; break;
    case DAMAGED_VERTICAL_TRUSS:   id = MISSING_VERTICAL_TRUSS;   break;
    case DAMAGED_CROSS_TRUSS:      id = MISSING_CROSS_TRUSS;      break;
    default:                       id = MISSING_FLOOR;            break;
    }
    map->SetTileId(p, id);
}

void Map::Explode(const Vector3 &at, float radius, bool floors)
{
    core::Size  tilesz = MapModel->GetTileSize();    
    core::Point center = GetTileLocationAt(v2(at));
    core::Rectangle r(int(center.x - radius / tilesz.width  - 1),
                      int(center.y - radius / tilesz.height - 1),
                      int(radius * 2 / tilesz.width  + 2),
                      int(radius * 2 / tilesz.height + 2));
    float radius_sq = (radius / tilesz.width) * (radius / tilesz.width);
    for (int j = 0; j < r.height; j++)
    {
        for (int i = 0; i < r.width; i++)
        {
            core::Point p(i + r.x, j + r.y);
            float dsq = (float)((p.x - center.x) * (p.x - center.x) + (p.y - center.y) * (p.y - center.y));
            if (dsq < radius_sq)
            {
                int tid = MapModel->GetTileId(p);
                switch (tid)
                {
                case HORIZONTAL_BROKEN_1:        if (!OnEdge(MapModel, p, 0)) MapModel->SetTileId(p, HORIZONTAL_BROKEN_0); break;
                case VERTICAL_BROKEN_1:          if (!OnEdge(MapModel, p, 0)) MapModel->SetTileId(p, VERTICAL_BROKEN_0);   break;                        
                case HORIZONTAL:                 MapModel->SetTileId(p, HORIZONTAL_BROKEN_1); break;
                case VERTICAL:                   MapModel->SetTileId(p, VERTICAL_BROKEN_1);   break;
                case HORIZONTAL_BROKEN_0:
                case VERTICAL_BROKEN_0:                        
                case FLOOR_TILE1:
                case FLOOR_TILE2:                if (floors) SetDamagedTile(MapModel, p); break;
                case DAMAGED_HORIZONTAL_TRUSS:
                case DAMAGED_VERTICAL_TRUSS:
                case DAMAGED_CROSS_TRUSS:                        
                case DAMAGED_FLOOR_1:  
                case DAMAGED_FLOOR_2:
                case DAMAGED_FLOOR_3:  
                case DAMAGED_FLOOR_4:            if (floors) SetMissingTile(MapModel, p); break;                        
                }
            }
        }
    }
}

bool Map::IsCollidableTile(int id)
{
    /*tmp*/ if (id == 0) return true;
    if (id > 1023) return false;
    if (id >= 3 && id <= 14)
        return true;
    return id == -1 || (id > 15 && id != 200 && id != 201 &&
                        id != VERTICAL_BROKEN_0 && id != HORIZONTAL_BROKEN_0 && 
                        id != DAMAGED_FLOOR_1 && id != DAMAGED_FLOOR_2 && 
                        id != DAMAGED_FLOOR_3 && id != DAMAGED_FLOOR_4 &&
                        id != DAMAGED_HORIZONTAL_TRUSS && id != DAMAGED_VERTICAL_TRUSS &&
                        id != DAMAGED_CROSS_TRUSS &&
                        id != MISSING_HORIZONTAL_TRUSS && id != MISSING_VERTICAL_TRUSS &&
                        id != MISSING_CROSS_TRUSS);
}