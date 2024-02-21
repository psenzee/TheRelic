/*
 *  Level.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Level.h"
#include "GameState.h"
#include "Player.h"
#include "BallCharacter.h"

#include "map/Map.h"
#include "map/MapGenerator.h"
#include "map/TileMap.h"

#include "render/OverheadCamera.h"
#include "render/Renderable.h"
#include "events/Event.h"
#include "time/Timer.h"

#include "core/xna.h"
#include "core/random.h"

class XmlElement;

LevelFile::LevelFile(const char *path, const char *name) : time(30), tilt(0.0f, 0.0f)
{
        /*
            XmlDocument doc = new XmlDocument();
            doc.Load(path);
            XmlElement root = XmlUtil.GetFirstElement(doc);
            configuration = XmlUtil.GetElementWithKeyEqualTo(root, "level", "name", name);
            
            this.name        = name;
            this.background  = XmlUtil.GetExpectedAttribute(configuration, "background", "background");
            this.description = XmlUtil.GetAttribute(configuration, "description");
            this.time        = XmlUtil.GetInt(configuration, "time");
            this.tilt        = XmlRenderableReader.ReadPoint2(
                                                              XmlUtil.GetFirstElementByTagName(configuration, "tilt"));
         */
}

Level::Level(GameState *state, const char *path, int level) : state(state), levelnumber(level), locked(false), exitstate(EXIT_VALID)
{
    options = new LevelFile(path, "[random]");
    MapGenerator mg(state->services);
//    const float EXPANSION = 0.33f;
    const float EXPANSION = 0.25f;
    int seed = 0;
    if (state->mode == GameState::MAZES_ONLY)
        seed = GetCurrentTimeMs();
    map = mg.GenerateMap(level, core::Size(4 + int(level * EXPANSION), 4 + int(level * EXPANSION)), options->configuration, "[random]", seed);
}

Level::~Level()
{
    delete map;    
    map = 0;
    delete options;
    options = 0;
}

core::Size Level::GetSize()
{ 
    return map->GetMapModel()->GetMapSizeInPixels();
}
        
void Level::Update(const GameTime &time)
{
    if (map)
        map->Update(time);
}
        
void Level::Draw(const GameTime &time, const OverheadCamera &camera)
{
    if (map)
        map->Draw(time, camera);
}

void Level::SetExitState(ExitState state)
{
    exitstate = state;
/*
    switch (state)
    {
        case EXIT_VALID:   map->SetEndTile(locked ? Map::LOCKED_END_TILE : Map::END_TILE); break;
        case EXIT_INVALID: map->SetEndTile(Map::INVALID_END_TILE);                         break;
    }
*/    
    if (locked)
        map->SetEndTile(Map::LOCKED_END_TILE);
    else
    {
        switch (state)
        {
        case EXIT_VALID:   map->SetEndTile(Map::END_TILE);         break;
        case EXIT_INVALID: map->SetEndTile(Map::INVALID_END_TILE); break;
        }
    }
}

void Level::UnlockLevelExit()
{
    locked = false;
    SetExitState(exitstate); // force update of the tile
}

void Level::LockLevelExit()
{
    locked = true;
    SetExitState(exitstate); // force update of the tile
}

LevelSequence::LevelSequence(GameState *state) : random(354668 /*Arbitrary, but constant seed*/), index(0), currentIndex(1), state(state)
{
    memset(levels, 0, sizeof(levels));
}
    
void LevelSequence::RunEvents()
{
    ITileInstance *tile = state->GetPlayerTileAt();
    if (!tile)
        return;
    Renderable *r = (Renderable *)tile->GetRenderableObject();
    if (r)
    {
        std::vector<Event *> &events = r->GetEvents();
        if (!events.empty())
            for (std::vector<Event *>::const_iterator i = events.begin(), e = events.end(); i != e; ++i)
                state->RunEvent((*i)->text.c_str());        
    }
}
        
void LevelSequence::LoadNext()
{
    Shift();
    Vector3 position;
    if (levels[0])
    {
        position = levels[0]->GetMap()->GetPosition();
        position.z = -128.0f;
        levels[0]->GetMap()->SetPosition(position);
    }
    for (int i = currentIndex; i < LEVEL_COUNT; i++)
    {
        Vector3 position;
        if (levels[i])
            position = levels[i]->GetMap()->GetPosition();
        else
        {
            levels[i] = Load(index - currentIndex + i);
            if (levels[i] && levels[i - 1])
                position = levels[i - 1]->GetMap()->GetPosition() + (levels[i - 1]->GetMap()->GetEndPoint() - levels[i]->GetMap()->GetStartPoint());
        }
        position.z = (i - currentIndex) * 128.0f;
        levels[i]->GetMap()->SetPosition(position);
    }
    index++;
}

void LevelSequence::CompleteLoad(int levelNumber)
{
    for (int i = 0; i < LEVEL_COUNT; i++)
    {
        delete levels[i];
        levels[i] = 0;
    }
    index = levelNumber;
    levels[0] = Load(index - currentIndex);
    Vector3 position = levels[0]->GetMap()->GetPosition();
    position.z = -128.0f;
    levels[0]->GetMap()->SetPosition(position);
    for (int i = currentIndex; i < LEVEL_COUNT; i++)
    {
        if (index - currentIndex + i != 0) // the 0th map is nothing..
        {
            Vector3 position;
            levels[i] = Load(index - currentIndex + i);
            if (levels[i] && levels[i - 1])
                position = levels[i - 1]->GetMap()->GetPosition() + (levels[i - 1]->GetMap()->GetEndPoint() - levels[i]->GetMap()->GetStartPoint());
            position.z = (i - currentIndex) * 128.0f;
            levels[i]->GetMap()->SetPosition(position);
        }
    }
    index++;
}
        
void LevelSequence::ReloadCurrent()
{
    Vector3 position = levels[currentIndex]->GetMap()->GetPosition();
    delete levels[currentIndex];
    levels[currentIndex] = 0;
    levels[currentIndex] = Load(index - 1);
    levels[currentIndex]->GetMap()->SetPosition(position);
}

void LevelSequence::Draw(const GameTime &time)
{
    if (!GetLevel())
        return;
    for (int i = LEVEL_COUNT - 1; i >= 0; i--)
        if (levels[i])
            levels[i]->Draw(time, *(state->camera));
}
        
void LevelSequence::Update(const GameTime &time)
{
    for (int i = 0; i < LEVEL_COUNT; i++)
        if (levels[i])
            levels[i]->Update(time);
}
    
void LevelSequence::Shift()
{
    if (levels[0])
        delete levels[0];
    for (int i = 1; i < LEVEL_COUNT; i++)
        levels[i - 1] = levels[i];
    levels[LEVEL_COUNT - 1] = 0;
}    
    
Level *LevelSequence::Load(int index)
{
    return (index < 0) ? 0 : new Level(state, "LevelRandom.xml", index);
}
