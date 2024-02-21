/*
 *  CoreTileMap.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 12/17/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "CoreTileMap.h"
#include <stdio.h>
#include <string.h>

CoreTileMap::CoreTileMap(const core::Size &size) : size(size)
{
    Create(size, 0);
}

CoreTileMap::CoreTileMap(const core::Size &size, int32_t initialValue) : size(size)
{
    Create(size, initialValue);
}

CoreTileMap::~CoreTileMap()
{
    delete [] data;
    data = 0;
}

void CoreTileMap::Reverse()
{
    int32_t area = size.width * size.height;
    for (int32_t i = 0, sz = area / 2; i < sz; i++)
    {
        int32_t e = area - i - 1, t = 0;
        t = data[i];
        data[i] = data[e];
        data[e] = t;
    }
}

int32_t CoreTileMap::GetTileId(int32_t x, int32_t y) const
{
    if (x < 0 || y < 0 || x >= size.width || y >= size.height)
        return 0;
    return data[y * size.width + x];
}

bool CoreTileMap::SetTileId(int32_t x, int32_t y, int32_t id)
{
    if (x < 0 || y < 0 || x >= size.width || y >= size.height || id < 0)
        return false;
    data[y * size.width + x] = id;
    return true;
}

core::Size CoreTileMap::GetSizeInTiles() const
{
    return size;
}

bool CoreTileMap::Read(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    bool success = false;
    if (!file)
        return false;
    int32_t width = 0, height = 0;
    if (!fread(&width,  1, sizeof(int32_t), file))
        goto error;
    if (!fread(&height, 1, sizeof(int32_t), file))
        goto error;
    Create(core::Size(width, height), 0);
    if (!fread(data, 1, width * height * sizeof(int32_t), file))
        goto error;
    for (int32_t i = 0, sz = width * height; i < sz; i++)
        data[i] &= 0x7fffffff; // make sure we don't overflow..
    success = true;
error:
    fclose(file);
    return success;
}

void CoreTileMap::Create(const core::Size &size, int32_t value)
{
    this->size = size;
    int32_t area = size.width * size.height;
    data = new int32_t [area];
    for (int32_t i = 0; i < area; i++)
        data[i] = value;
}
