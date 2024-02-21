#ifndef _OBJECTTILE_H
#define _OBJECTTILE_H

#include "ITileMap.h"

class IGameObject;
class GameTime;

class ObjectTile : public ITile
{
public:
        
    ObjectTile(IGameObject *object, int id);
    ~ObjectTile();
        
    IGameObject   *GetGameObject();
    int            GetId() const;
    ITileInstance *CreateTileInstance();
    void           Update(const GameTime &time);
    bool           HasEvent(const char *name) const;
        
private:
        
    IGameObject   *object;
    ITileInstance *shared;
    int            id;
};

#endif // _OBJECTTILE_H