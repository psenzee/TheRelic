/*
 *  ContentCache_.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

/*
 */

#ifndef _CONTENTCACHE_H
#define _CONTENTCACHE_H

#include <map>
#include <string>

class DeviceModel;
class DeviceTexture;

class ContentCache
{
public:
        
    ContentCache();
    ~ContentCache();
        
    DeviceModel   *GetModel(const char *name);
    DeviceTexture *GetTexture(const char *name);
        
private:
        
    std::map<std::string, DeviceModel   *> models;
    std::map<std::string, DeviceTexture *> textures;
};

#endif // _CONTENTCACHE_H
