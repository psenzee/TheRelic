

/*
 *  MarkedMap.cpp
 *  GLGravity
 *
 *  Created by Paul Senzee on 1/28/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "MarkedMap.h"

MarkedMap::MarkedMap(const ITileMap *map) : map(map), data(map->GetSizeInTiles().width, map->GetSizeInTiles().height)
{
}

bool MarkedMap::FindClosestUnmarkedFloor(const core::Point &start, core::Point &point)
{
    int         distancesq = 0x7fffffff;
    bool        found      = false;
    core::Size  sz         = map->GetSizeInTiles();
    
    for (int j = 0; j < sz.height; j++)
    {
        for (int i = 0; i < sz.width; i++)
        {
            core::Point p(i, j);
            if (IsUnmarkedFloor(p))
            {
                int dx  = start.x - i,
                    dy  = start.y - j,
                    dsq = dx * dx + dy * dy;
                if (dsq < distancesq)
                {
                    distancesq = dsq;
                    point      = p;
                    found      = true;
                }
            }
        }
    }
    return found;
}

/*

bool MarkedMap::FindUnmarkedFloorRing(const core::Rectangle &area, core::Point &point)
{
    core::Size      sz(map->GetSizeInTiles());
    core::Rectangle r (Overlap(area, core::Rectangle(0, 0, sz.width, sz.height)));
    int x0 = r.x, y0 = r.y, x1 = r.width + r.x, y1 = r.height + r.y;
    core::Point p;
    for (int i = x0; i < x1; i++)
    {
        p.x = i;
        p.y = y0; if (IsMarkedFloor(p)) { point = p; return true; }
        p.y = y1; if (IsMarkedFloor(p)) { point = p; return true; }
    }
    for (int i = y0; i < y1; i++)
    {
        p.y = i;
        p.x = x0; if (IsMarkedFloor(p)) { point = p; return true; }
        p.x = x1; if (IsMarkedFloor(p)) { point = p; return true; }
    }
    return false;
}

bool MarkedMap::FindUnmarkedFloor(const core::Rectangle &area, core::Point &point)
{
    core::Size      sz(map->GetSizeInTiles());
    core::Rectangle r (Overlap(area, core::Rectangle(0, 0, sz.width, sz.height)));
    for (int j = r.y; j < r.height; j++)
        for (int i = r.x; i < r.width; i++)
            if (IsMarkedFloor(core::Point(i, j)))
            { point = p; return true; }
    return false;
}

bool MarkedMap::FindUnmarkedFloor(const core::Point &start, core::Point &point)
{
    core::Size mapsz(map->GetSizeInTiles());
    for (int i = 0, sz = maxi(mapsz.width, mapsz.height); i < sz; i++)
        if (FindUnmarkedFloorRing(Concentric(start, i), point))
            return true;
    return false;
}

*/
