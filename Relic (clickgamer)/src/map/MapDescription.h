#ifndef _MAP_DESCRIPTION_H
#define _MAP_DESCRIPTION_H

#include <string>
#include <vector>
#include <map>

#include "core/core.h"
#include "fast/Allocator.h"

class XmlElement;

class MapDescription
{
public:
    
  //CLASS_NEW_DELETE() // $TODO can't do this here until we provide an array allocator, need to do that

    int                                level;
    int                                seed;
    bool                               render;
    bool                               events;
    core::Size                         size;
    std::string                        name;
    std::string                        filename;
    std::string                        tileset;
    std::map<std::string, std::string> properties;
    
    inline MapDescription() : level(0), seed(0), render(true), events(true) {}
};

bool ReadMapDescription(XmlElement *xml, std::vector<MapDescription> &maps);
void ReadMapDescriptions(XmlElement *xml, std::vector<MapDescription> &maps);

#endif // _MAP_DESCRIPTION_H