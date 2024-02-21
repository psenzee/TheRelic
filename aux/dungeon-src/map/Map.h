/*
 *  Map.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/29/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _MAP_SENZEE_H
#define _MAP_SENZEE_H

#include "core/xna.h"
#include "core/random.h"
#include "core/aabox.h"

class XmlElement;
class ITileMapModel;
class LocalGameServices;
class IRenderable;
class OverheadCamera;

class MapFile
{
public:
    const char *name,
               *description,
               *file,
               *tiles;
    
    MapFile(XmlElement *level, const char *name);
};

class Map
{
public:
    enum SpecialTile
    {
        FLOOR_TILE1      = 1,
        FLOOR_TILE2      = 5,
        START_TILE       = 4,
        END_TILE         = 2,
        LOCKED_END_TILE  = 200,
        INVALID_END_TILE = 201,
        TREE_TILE1       = 7,
        TREE_TILE2       = 6,
        PICKUP_2X        = 8,
        PICKUP_3X        = 9,
        SWIRL            = 10
    };
        
    ITileMapModel     *MapModel;
    MapFile           *Options;
    LocalGameServices *Services;
    core::Point        Start,
                       End;
    
    Map(LocalGameServices *services, ITileMapModel *model, MapFile *options);
    ~Map();    
    
    inline ITileMapModel       *GetMapModel()                { return MapModel; }
    inline const core::Point   &GetStartTileLocation() const { return Start; }
    inline const core::Point   &GetEndTileLocation()   const { return End; }    
    
    Vector3                     GetStartPoint()        const;  // returns world - Position
    Vector3                     GetEndPoint()          const;  // returns world - Position    
        
    void                        SetPlayerTile(const Vector2 &position, int id);
    void                        SetEndTile(int id);
    
    void                        ClearPickup(const Vector2 &position);
    void                        ClearEndArea();
                
    core::Point                 FindEvent(const char *name) const;
    Vector3                     GetLookAt(const Vector2 &player) const; // returns world
    Vector3                     GetSize() const;
    
    int                         GetTileIdAt(const Vector2 &p) const;
    core::Point                 GetTileLocationAt(const Vector2 &p) const; // expects world coordinates
    
    void                        Draw(const GameTime &time, const OverheadCamera &camera);
    void                        Update(const GameTime &time);    
    Vector3                     GetRandomFloorPosition(core::Random &r) const; // returns world
    
    Vector3                     GetPointInWorld(const core::Point &p)   const;
    Vector3                     GetEventPointInWorld(const char *event) const; // returns world
    
    const AABox                &GetBounds()                          const { return bounds; }
    const Vector3              &GetPosition()                        const { return Position; }
    void                        SetPosition(const Vector3 &position);
    void                        MovePosition(const Vector3 &offset);
    
    static bool                 IsFloorTile(int id);    
    static bool                 IsCollidableTile(int id);
    
    void                        Explode(const Vector3 &world, float radius, bool floors);
    
private:
    
    Vector3                     GetNormalizedPosition(const Vector3 &at) const;
    
    static SpecialTile          GetFloorTileAt(const core::Point &at);
    
    Vector3                     Position;    
    AABox                       basebounds,
                                bounds;
    std::vector<core::Point>    dynamic;
    std::vector<core::Point>    floors;
};

#endif // _MAP_SENZEE_H