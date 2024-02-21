/*
 *  DeviceTileSet_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DeviceTileSet.h"

#include "core/strs.h"
#include "core/global.h"
#include "xml/XmlReadContext.h"
#include "xml/XmlObjectReader.h"
#include "render/LocalGameServices.h"

DeviceTileSet::DeviceTileSet(XmlReadContext *context, const core::Size &size)
: RenderableTileSet(CreateGameObjects(context), size), context(context)
{
}

std::vector<Renderable *> DeviceTileSet::CreateGameObjects(XmlReadContext *context)
{
    enum { MAXIMUM_TILE_COUNT = 1024 * 2 };
    std::vector<Renderable *> objects;
    for (int i = 0; i < MAXIMUM_TILE_COUNT; i++)
        objects.push_back(0); // $todo, come on - do this the right way
    const char prefix[] = "Tile.";
    int prefix_sz = sizeof(prefix) - 1;
    for (std::map<std::string, Renderable *>::iterator i = context->objects.begin(), e = context->objects.end(); i != e; ++i)
    {
        const char *s = (*i).first.c_str();
        if (starts(s, prefix, true))
        {
            s += prefix_sz;
            int index = atoi(s);
            if (index > 0)
                objects[index] = (*i).second;
        }
    }
    return objects;
}

std::map<std::string, ITileSet *> DeviceTileSetLoader::cache;

ITileSet *DeviceTileSetLoader::GetTileSet(const char *name, const char *common_name, LocalGameServices *services)
{
    ITileSet *ts = cache[name];
    if (!ts)
    {    
        XmlReadContext *context = new XmlReadContext(services->cache);
        XmlObjectReader::ReadText(globalReadTextFile(name),        context);
        if (common_name)
            XmlObjectReader::ReadText(globalReadTextFile(common_name), context);
        ts = new DeviceTileSet(context, core::Size(32, 32));
        cache[name] = ts;
    }
    return ts;
}
