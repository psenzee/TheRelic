/*
 *  Level.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEVEL_H
#define _LEVEL_H

#include "core/xna.h"
#include "core/random.h"

class XmlElement;
class Map;
class GameState;
class GameTime;
class OverheadCamera;

class LevelFile    
{
public:
        
    XmlElement *configuration;
        
    const char *name, 
               *description, 
               *background;
    int         time;
    Vector2     tilt;
        
    LevelFile(const char *path, const char *name);
};

class Level
{
public:    
    
    enum ExitState { EXIT_INVALID, EXIT_VALID };

    Level(GameState *state, const char *path, int level);
    ~Level();    
    
    core::Size        GetSize();
    
    inline Map       *GetMap()               { return map; }
    inline LevelFile *GetOptions()           { return options; }
    inline int        GetLevelNumber() const { return levelnumber; }
    
    void              SetExitState(ExitState state);
    
    void              Update(const GameTime &time);
    void              Draw(const GameTime &time, const OverheadCamera &camera);

    void              LockLevelExit();    
    void              UnlockLevelExit();
    bool              IsLocked() const { return locked; }
    
private:
    
    Map       *map;
    LevelFile *options;
    GameState *state;
    int        levelnumber;
    bool       locked;
    ExitState  exitstate;
    
};

class LevelSequence
{
    enum { LEVEL_COUNT = 3 };
        
    int          index;
    Level       *levels[LEVEL_COUNT];    
    int          currentIndex;    
    core::Random random; 
        
public:
        
    GameState    *state;
        
    LevelSequence(GameState *state);
        
    inline Level *GetLevel()         { return levels[currentIndex]; }
    inline Level *GetPreviousLevel() { return levels[currentIndex - 1]; }
    inline int    GetLevelNumber()   { return index; }
        
    void          RunEvents();
        
    void          LoadNext();
    void          CompleteLoad(int levelNumber);
    void          ReloadCurrent();
        
    void          Draw(const GameTime &time);    
    void          Update(const GameTime &time);
        
private:
        
    void   Shift();        
    Level *Load(int index);
};

#endif // _LEVEL_H