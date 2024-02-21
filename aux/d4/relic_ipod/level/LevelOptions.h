/*
 *  LevelOptions.h
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/30/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _LEVELOPTIONS_H
#define _LEVELOPTIONS_H

#include "core/core.h"
#include "map/MapDescription.h"

class XmlElement;

#include <vector>

class LevelOptions
{
public:
        
    XmlElement                    *configuration;
        
    const char                    *name, 
                                  *description;
    Vector2                        tilt;
    std::vector<MapDescription>    maps;
        
    LevelOptions(const char *path);
};

#endif // _LEVELOPTIONS_H