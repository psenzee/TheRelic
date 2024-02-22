#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <map>
#include <vector>
#include <string>

#define STATIC_DICTIONARY 1

class Dictionary
{
public:

    bool               Insert(const char *text);
    int                InsertAll(const char **text); // insert until null

    void               Clear();

    const char        *GetText(int index)         const;
    int                GetId(const char *text)    const;

private:

#ifndef STATIC_DICTIONARY
    struct CStringComparator
    {
        inline bool operator()(const char *a, const char *b) const { return strcmp(a, b) < 0; }
    };

    typedef std::map<const char *, int, CStringComparator> map_t;

    std::vector<const char *> mText;
    map_t                     mMap;
#endif
};

#endif // _DICTIONARY_H