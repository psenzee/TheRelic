#ifndef _MAP_PDS_H
#define _MAP_PDS_H

#include "core/core.h"
#include "core/random.h"
#include "core/aabox.h"

class XmlElement;
class ITileMapModel;
class RenderContext;
class IRenderable;
class OverheadCamera;
class ITile;

#include "gamecore/ICollidable.h"

class Map
{
public:
    
    Map(ITileMapModel *model);
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
  
    ITile                      *GetTileAt(const Vector2 &p);
    void                        SetTileIdAt(const Vector2 &p, int id);
    int                         GetTileIdAt(const Vector2 &p) const;
    core::Point                 GetTileLocationAt(const Vector2 &p) const; // expects world coordinates
    
    void                        Draw(RenderContext &context, const GameTime &time);
    void                        Update(RenderContext &context, const GameTime &time);
//  Vector3                     GetRandomFloorPosition(core::Random &r) const; // returns world
    
    Vector3                     GetPointInWorld(const core::Point &p)   const;
    Vector3                     GetEventPointInWorld(const char *event) const; // returns world
    
    const AABox                &GetBounds()                          const { return bounds; }
    const Vector3              &GetPosition()                        const { return Position; }
    void                        SetPosition(const Vector3 &position);
    void                        MovePosition(const Vector3 &offset);

    ICollidable::Classification Collision(const Vector3 &p, float radius, Vector3 &resolve);
    
//  void                        Explode(const Vector3 &world, float radius, bool floors);

    const core::Rectangle      &GetRenderRange() { return mRenderRange; }
   
private:
    
    Vector3                     GetNormalizedPosition(const Vector3 &at) const;

        
    ITileMapModel              *MapModel;
    core::Point                 Start,
                                End;

    Vector3                     Position;
    AABox                       basebounds,
                                bounds;
//  std::vector<core::Point>    dynamic;
//  std::vector<core::Point>    floors;

    core::Rectangle             mRenderRange;
};

#endif // _MAP_PDS_H