/*
 *  TileInstanceMap.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TileInstanceMap.h"
#include <string.h>

TileInstanceMap::TileInstanceMap(const core::Size &size) : size(size)
{
    Create(size);
}


TileInstanceMap::~TileInstanceMap()
{
    for (int i = 0, sz = size.width * size.height; i < sz; i++)
        if (data[i]) data[i]->Release();
    delete [] data;
    data = 0;
}

ITileInstance *TileInstanceMap::GetTileInstance(int x, int y)
{
    if (x < 0 || y < 0 || x >= size.width || y >= size.height)
        return 0;
    return data[y * size.width + x];
}

bool TileInstanceMap::SetTileInstance(int x, int y, ITileInstance *instance)
{
    if (x >= size.width || y >= size.height)
        return false;
    data[y * size.width + x] = instance;
    return true;
}

core::Size TileInstanceMap::GetSizeInTiles() const
{
    return size;
}

void TileInstanceMap::Create(const core::Size &size)
{
    int area = size.width * size.height;
    data = new ITileInstance * [area];
    memset(data, 0, sizeof(ITileInstance *) * area);
}
