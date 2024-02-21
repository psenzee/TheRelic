#include "MapDescription.h"
#include "core/simplexml.h"

bool ReadMapDescription(XmlElement *xml, std::vector<MapDescription> &maps)
{
    if (strcmp(xml->name, "map") != 0) 
        return false;
    MapDescription map;
    const char *name = XmlUtil::GetAttributeValue(xml, "name");
    if (name && strlen(name) > 0)
        map.name = name;
    map.level       = XmlUtil::GetInt(xml, "level");
    map.seed        = XmlUtil::GetInt(xml, "seed");
    map.size.width  = XmlUtil::GetInt(xml, "width");
    map.size.height = XmlUtil::GetInt(xml, "height");
    map.tileset     = XmlUtil::GetExpectedAttributeValue(xml, "tileset");
    maps.push_back(map);
    return false;
}

void ReadMapDescriptions(XmlElement *xml, std::vector<MapDescription> &maps)
{
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
        if (strcmp(xml->children[i]->name, "map") == 0)
            ReadMapDescription(xml->children[i], maps);
}
