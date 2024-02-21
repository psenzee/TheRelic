#include "XmlUtil.h"

#include "core/simplexml.h"
#include "core/format.h"
#include "core/global.h"

#include <exception>
#include <string.h>

XmlElement *XmlUtil::GetFirstElement(XmlElement *e)
{
    return elementAt(e, 0);
}

XmlElement *XmlUtil::GetFirstElement(XmlElement *e, const char *name)
{
    return elementAt(e, findElementIndex(e, name));    
}

XmlElement *XmlUtil::GetFirstElementWithKeyValue(XmlElement *e, const char *name, const char *key, const char *value)
{
    return elementAt(e, findElementIndexWithKeyValue(e, name, key, value));
}

const char *XmlUtil::GetAttributeValue(XmlElement *xml, const char *key, const char *defaultValue)
{
    XmlAttribute *a = attributeAt(xml, findAttributeIndex(xml, key));
    return a ? a->value : defaultValue;
}

const char *XmlUtil::GetExpectedAttributeValue(XmlElement *xml, const char *name)
{
    const char *attr = GetAttributeValue(xml, name);
    if (!attr)
        globalErrorHandler("Attribute '%s' expected in element", name);
    return attr;
}

const char *XmlUtil::GetText(XmlElement *xml)
{
    for (int i = 0, sz = xml->childrenCount; i < sz; i++)
    {
        XmlElement *e = xml->children[i];
        if (e->type == XmlElement::TEXT)
            return e->text;
    }
    return 0;
}

bool XmlUtil::GetBool(XmlElement *xml, const char *name, bool defaultValue)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        if      (strcmp(s, "true")  == 0)  return true;
        else if (strcmp(s, "false") == 0) return false;
    }
    return defaultValue;
}

bool XmlUtil::GetBool(XmlElement *xml, const char *name)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        if      (strcmp(s, "true")  == 0)  return true;
        else if (strcmp(s, "false") == 0) return false;
    }
    globalErrorHandler("Expected 'true' or 'false' for boolean attribute '%s'", name);
    return false;
}

float XmlUtil::GetFloat(XmlElement *xml, const char *name)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        char *e = (char *)s;
        float v = (float)strtod(s, &e);
        if (e && s != e)
            return v;
    }
    globalErrorHandler("Expected numeric value for float attribute '%s'", name);
    return 0.0f;
}

float XmlUtil::GetFloat(XmlElement *xml, const char *name, float defaultValue)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        char *e = (char *)s;
        float v = (float)strtod(s, &e);
        if (e && s != e)
            return v;
    }
    return defaultValue;
}

int XmlUtil::GetInt(XmlElement *xml, const char *name)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        char *e = (char *)s;
        int v = (int)strtol(s, &e, 10);
        if (e && s != e)
            return v;
    }
    globalErrorHandler("Expected integer value for integer attribute '%s'", name);
    return 0;
}

int XmlUtil::GetInt(XmlElement *xml, const char *name, int defaultValue)
{
    const char *s = GetAttributeValue(xml, name);
    if (s)
    {
        char *e = (char *)s;
        int v = (int)strtol(s, &e, 10);
        if (e && s != e)
            return v;
    }
    return defaultValue;
}
