#include "LevelOptions.h"
#include "map/MapDescription.h"

#include "core/core.h"
#include "core/random.h"
#include "core/global.h"
#include "core/simplexml.h"
#include "core/file.h"
#include "core/core_assert.h"

#include "xml/XmlUtil.h"

class XmlElement;

LevelOptions::LevelOptions(const char *path) : configuration(0), name(0), description(0), tilt(0.0f, 0.0f)
{
    core_assert(file::path_exists(globalTranslatePath(path)) && "file must exist!");
    XmlElement *xml = parse(globalReadTextFile(path));
    configuration = xml;
    if (!xml)
        printf("ERROR: Unable to load level '%s'!\n", path);
    else
    {
        XmlElement *root = XmlUtil::GetFirstElement(xml, "level");
        if (!root)
            printf("ERROR: <level /> element not found in '%s'!\n", path);
        else
        {
            name        = XmlUtil::GetExpectedAttributeValue(root, "name");
            description = XmlUtil::GetExpectedAttributeValue(root, "description");
            ReadMapDescriptions(root, maps);
        }
    }
}