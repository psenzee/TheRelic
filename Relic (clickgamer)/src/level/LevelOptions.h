#ifndef _LEVELOPTIONS_H
#define _LEVELOPTIONS_H

#include "core/core.h"
#include "map/MapDescription.h"
#include "fast/Allocator.h"

class XmlElement;

#include <vector>

class LevelOptions
{
public:
    
    CLASS_NEW_DELETE()
        
    XmlElement                    *configuration;
        
    const char                    *name, 
                                  *description;
    Vector2                        tilt;
    std::vector<MapDescription>    maps;
        
    LevelOptions(const char *path);
};

#endif // _LEVELOPTIONS_H