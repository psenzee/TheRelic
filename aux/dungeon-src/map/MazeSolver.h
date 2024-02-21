#ifndef _MAZESOLVER_H
#define _MAZESOLVER_H

#include "core/xna.h"

#include <string.h>
#include <stdio.h>

class Map;

bool Reachable(Map *map, const core::Point &start, const core::Point &end);

#endif // _MAZESOLVER_H