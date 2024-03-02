#ifndef _OBJECTTILE_H
#define _OBJECTTILE_H

#include "ITileMap.h"
#include "fast/Allocator.h"

class IGameObject;
class GameTime;

class ObjectTile : public ITile
{
public:
        
    CLASS_NEW_DELETE();
        
    ObjectTile(IGameObject *object, int id);
    ~ObjectTile();
        
    IGameObject   *GetGameObject();
    int            GetId() const;
    void           Update(const GameTime &time);
    bool           HasEvent(const char *name) const;
        
private:
        
    IGameObject   *mObject;
    int            mId;
};

#endif // _OBJECTTILE_H