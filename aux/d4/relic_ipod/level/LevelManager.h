/*
 *  LevelManager.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEVELMANAGER_H
#define _LEVELMANAGER_H

#include "core/core.h"
#include "core/random.h"

class XmlElement;
class Map;
class GameTime;
class OverheadCamera;
class RenderContext;
class ContentLoader;
class IEventListener;
class Multiplayer;
class Level;

class LevelManager
{       
    int             index;
    Level          *level;
    core::Random    random;
    ContentLoader  *loader;
    Multiplayer    *mMultiplayer;
        
public:
       
       
    LevelManager(ContentLoader *loader, Multiplayer *multiplayer);
        
    inline Level  *GetLevel()         { return level; }
    inline int     GetLevelNumber()   { return index; }
        
    void           RunEvents(const Vector3 &position, IEventListener &listener);
        
    void           LoadNext();
    void           ReloadCurrent();
        
    void           Draw(RenderContext &context, const GameTime &time);
    void           Update(RenderContext &context, const GameTime &time, bool paused);
        
private:
        
    Level         *Load(int index);
};

#endif // _LEVELMANAGER_H