#ifndef _PLACENAMEGENERATOR_H
#define _PLACENAMEGENERATOR_H

#include "core/random.h"

class PlaceNameGenerator
{
public:

    PlaceNameGenerator();

    const char *GetPlaceName(core::Random &random);
    const char *GetPlaceNameAt(int seed, int x, int y)
    {
        enum { PRIME = 16381 }; // pretty hi prime number
        core::Random r(seed + y * PRIME + x);
        return GetPlaceName(r);
    }

private:

    int mAdjectivesCount,
        mPlaceNamesCount,
        mNamesCount;

    enum { MAX_PLACENAME_SIZE = 1024 };

    char mName[MAX_PLACENAME_SIZE];

    const char *FormatName1(const char *adjective, const char *placeName, const char *name);
    const char *FormatName2(const char *adjective, const char *placeName, const char *name);
};

#endif // _PLACENAMEGENERATOR_H