#ifndef _NETDICTIONARY_H
#define _NETDICTIONARY_H

#include <map>
#include <vector>
#include <string>

struct CStringComparator
{
    inline bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; }
};

class NetDictionary
{
public:

    bool        Insert(const char *text);
    int         InsertAll(const char **text); // insert until null

    void        Clear();

    const char *GetText(int index)      const;
    int         GetId(const char *text) const;

private:

    typedef std::map<const char *, int, CStringComparator> map_t;

    std::vector<const char *> mText;
    map_t                     mMap;
};

void TestDictionary();

#endif // _NETDICTIONARY_H