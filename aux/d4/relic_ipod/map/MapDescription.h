#ifndef _MAP_DESCRIPTION_H
#define _MAP_DESCRIPTION_H

#include <string>
#include <vector>
#include <map>

#include "core/core.h"

class XmlElement;

class MapDescription
{
public:

    int                                level;
    int                                seed;
    core::Size                         size;
    std::string                        name;
    std::string                        filename;
    std::string                        tileset;
    std::map<std::string, std::string> properties;
    
    inline MapDescription() : level(0), seed(0) {}
};

bool ReadMapDescription(XmlElement *xml, std::vector<MapDescription> &maps);
void ReadMapDescriptions(XmlElement *xml, std::vector<MapDescription> &maps);

#endif // _MAP_DESCRIPTION_H