/*
 *  ContentCache_.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/16/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 */

#include "ContentCache.h"
#include "DeviceModel.h"
#include "DeviceTexture.h"

ContentCache::ContentCache()
{
}

ContentCache::~ContentCache()
{
}

DeviceModel *ContentCache::GetModel(const char *name)
{
    DeviceModel *dm = models[name];
    if (!dm)
    {    
        std::string path(name);
        path += ".obj";
        dm = new DeviceModel(path.c_str());
        models[name] = dm;
    }
    return dm;
}

DeviceTexture *ContentCache::GetTexture(const char *name)
{
    DeviceTexture *dt = textures[name];
    if (!dt)
    {    
        dt = new DeviceTexture(name);
        textures[name] = dt;
    }
    return dt;
}