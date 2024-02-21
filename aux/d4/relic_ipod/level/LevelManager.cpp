#include "LevelManager.h"
#include "Level.h"

#include "core/core.h"
#include "core/random.h"
#include "core/global.h"

#include "map/Map.h"
#include "map/MapLoader.h"
#include "map/MapDescription.h"
#include "events/Event.h"

class XmlElement;

LevelManager::LevelManager(ContentLoader *loader, Multiplayer *multiplayer) 
  : random(354668), index(0), loader(loader), level(0), mMultiplayer(multiplayer)
{
}
    
void LevelManager::RunEvents(const Vector3 &position, IEventListener &listener)
{
    if (level)
        level->RunEvents(position, listener);
}
        
void LevelManager::LoadNext()
{
    if (level)
        delete level;
    level = 0;
    level = Load(++index);
}
        
void LevelManager::ReloadCurrent()
{
    if (level)
        delete level;
    level = 0;
    level = Load(index);
}

void LevelManager::Draw(RenderContext &context, const GameTime &time)
{
    if (level)
        level->Draw(context, time);
}
        
void LevelManager::Update(RenderContext &context, const GameTime &time, bool paused)
{
    if (level)
        level->Update(context, time, paused);
}
    
Level *LevelManager::Load(int index)
{
    if (index < 0)
        return false;
    char filename[1024]; // should be plenty
    sprintf(filename, "level%d.xml", index);
    return new Level(loader, filename, index, mMultiplayer);
}
