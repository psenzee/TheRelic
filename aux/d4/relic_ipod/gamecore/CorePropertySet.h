#ifndef _COREPROPERTYSET_H
#define _COREPROPERTYSET_H

#include "IPropertySet.h"

#include <map>
#include <string>

class NullPropertySet : public IPropertySet
{
public:

    bool        SetProperty(const char *key, const char *value) { return false; }
    const char *GetProperty(const char *key) const              { return 0; }

    static NullPropertySet instance;
};

class CorePropertySet : public IPropertySet
{
public:

    bool        SetProperty(const char *key, const char *value);
    const char *GetProperty(const char *key) const;

private:

    std::map<std::string, std::string> mProperties;
};


#endif // _COREPROPERTYSET_H
