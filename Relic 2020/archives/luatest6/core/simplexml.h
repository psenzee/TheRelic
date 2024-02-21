#ifndef _SIMPLEXML_H
#define _SIMPLEXML_H

class XmlAttribute
{
public:
    const char *key,
               *value;

    XmlAttribute(const char *key, const char *value);
    ~XmlAttribute();
};

class XmlElement
{
public:

    enum Type { TAG, TEXT };

    Type                         type;
    const char                  *name,
                                *text;
    XmlAttribute               **attributes;
    int                          attributeCount;
    XmlElement                 **children;
    int                          childrenCount;

    XmlElement(Type type = TAG, const char *name = 0, const char *text = 0);
    ~XmlElement();
};

XmlElement   *parse(const char *text);
void          print(XmlElement *e);

int           findElementIndex(XmlElement *xml, const char *name, int start = 0);
int           findAttributeIndex(XmlElement *xml, const char *key, int start = 0);
bool          hasKeyValueEqualTo(XmlElement *xml, const char *key, const char *value, int start = 0);
int           findElementIndexWithKeyValue(XmlElement *xml, const char *name, const char *key, const char *value, int start = 0);

XmlElement   *elementAt(XmlElement *xml, int index);
XmlAttribute *attributeAt(XmlElement *xml, int index);

#endif // _SIMPLEXML_H