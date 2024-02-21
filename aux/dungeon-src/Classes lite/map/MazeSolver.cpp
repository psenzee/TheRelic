#include "MazeSolver.h"
#include "Map.h"
#include "ITileMap.h"

#include "core/global.h"

#include <string.h>
#include <stdio.h>

class Location
{
public:
    int x, y;

    inline Location(int x, int y) : x(x), y(y) {}

    inline Location operator+(const Location &u) const { return Location(x + u.x, y + u.y); }
    inline bool     operator!=(const Location &u) const { return x != u.x || y != u.y; }
    inline bool     operator==(const Location &u) const { return x == u.x || y == u.y; }
};

class Grid
{
public:
    int *data;
    int  width, height;

    Grid(int width, int height) : data(0), width(width), height(height)
    {
        data = new int [width * height];
        memset(data, 0, width * height * sizeof(int));
    }
    inline int  GetAt(int x, int y)        const { return data[x + y * width]; }
    inline void SetAt(int x, int y, int v)       { data[x + y * width] = v; }
    ~Grid()
    {
        delete [] data;
        data = 0;
    }
};

inline bool IsWalkable(const Grid &g, const Location &p)
{
    if (p.x < 0 || p.y < 0)
        return false;
    return (g.GetAt(p.x, p.y) & 1) == 0;
}

inline bool IsVisitable(const Grid &g, const Location &p)
{
    if (p.x < 0 || p.y < 0)
        return false;
    return (g.GetAt(p.x, p.y) & 3) == 0;
}

inline bool IsWalkable(const Grid &g, int x, int y)
{
    return IsWalkable(g, Location(x, y));
}

inline bool IsVisitable(const Grid &g, int x, int y)
{
    return IsVisitable(g, Location(x, y));
}

inline void Mark(Grid &g, const Location &p)
{
    if (p.x < 0 || p.y < 0)
        return;
    return g.SetAt(p.x, p.y, (g.GetAt(p.x, p.y) | 2));
}

//typedef std::vector<Location> Path;

/*
bool Solve(Grid &g, const Location &start, const Location &end, Path &path)
{
    if (!IsVisitable(g, start))
        return false;
    if (start == end)
    {
        path.push_back(start);
        return true;
    }
    else
    {
        Location directions[4] = { Location(0, -1), Location(1, 0), Location(0, 1), Location(-1, 0) };
        bool     solve[4]      = { false, false, false, false },
                 any           false;
        Path     paths[4];
        for (int i = 0; i < 4; i++)
            any |= (solve[i] = Solve(g, start + directions[i], end, paths[i]));
        if (!any)
        {
            Mark(g, 
            return false;
        }
        path.push_back(start);
    }
}
*/

bool CheckAndMarkOne(Grid &g, const Location &at, const Location &start, const Location &end, Location &exit)
{
    static Location dirs[] = { Location(0, -1), Location(1, 0), Location(0, 1), Location(-1, 0) };
    if (!IsVisitable(g, at))
        return false;
    int exits = 0;
    Location d(0, 0);
    for (int k = 0; k < 4; k++)
        if (IsVisitable(g, (d = at + dirs[k])))
            exits++;
    if (exits < 2 && at != start && at != end)
    {
        Mark(g, at);
        exit = d;
        return true;
    }
    return false;
}

bool CheckAndMarkLine(Grid &g, const Location &at, const Location &start, const Location &end)
{
    Location p = at;
    bool marked = false;
    while (CheckAndMarkOne(g, p, start, end, p))
        marked = true;
    return marked;
}

bool MarkDeadEnds(Grid &g, const Location &start, const Location &end)
{
    bool modified = false;
    for (int j = 0, h = g.height; j < h; j++)
    {
        for (int i = 0, w = g.width; i < w; i++)
        {
            modified |= CheckAndMarkLine(g, Location(i, j), start, end);
            modified |= CheckAndMarkLine(g, Location(w - i - 1,  h - j - 1), start, end);
        }
    }
    return modified;
}

bool Reachable(Grid &g, const Location &start, const Location &end);

void print(const Grid &g, const Location &start, const Location &end);

bool MarkReachablePath(Grid &g, const Location &start, const Location &end/*, bool exitIfUnreachable*/)
{
    do
    {
#ifdef _DEVELOPMENT_
        print(g, start, end);
#endif        
        if (/*exitIfUnreachable && */!Reachable(g, start, end))
            return false;
//        getch();
    }
    while (MarkDeadEnds(g, start, end));
    return Reachable(g, start, end);
}

bool IsNotSurrounded(Grid &g, const Location &p)
{
    /*
    for (int y = -1; y <= 1; y++)
        for (int x = -1; x <= 1; x++)
            if (y != 0 || x != 0)
                if (IsVisitable(g, Location(p.x + x, p.y + y)))
                    return true;
    return false;
    */
    return (IsVisitable(g, Location(p.x + 1, p.y    )) ||
            IsVisitable(g, Location(p.x,     p.y + 1)) ||
            IsVisitable(g, Location(p.x - 1, p.y    )) ||
            IsVisitable(g, Location(p.x,     p.y - 1)));
}

bool Reachable(Grid &g, const Location &start, const Location &end)
{
    if (!IsVisitable(g, start) || !IsVisitable(g, end))
        return false;
    return IsNotSurrounded(g, start) && IsNotSurrounded(g, end);
}
/*
typedef bool (*predicate_t)(const Grid &g, const Location &p);

Location GetClosest(const Grid &g, const Location &p, predicate_t)
{
    int r = (g.width > g.height ? g.width : g.height) * 2;
    for (int i = 0; i < r; i++)
    {

    }
}
*/
/*
int maze[] = {

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};
*/
int maze[] = {

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1,
    1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1,
    1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,
    1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};

/*
int maze[] = {

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};
*/
void print(const Grid &g, const Location &start, const Location &end)
{
    for (int j = g.height - 1; j >= 0; j--)
    {
        for (int i = g.width - 1; i >= 0; i--)
        {
            core::Point p(i, j);
            if      (start.x == p.x && start.y == p.y) printf("S");
            else if (end.x   == p.x && end.y   == p.y) printf("E");
            else                                       printf(IsVisitable(g, i, j) ? " " : "X");
        }
        printf("\n");
    }
    printf("\n");
}

bool Reachable(Map *m, const core::Point &start, const core::Point &end)
{
    ITileMapModel *model = m->GetMapModel();    
    core::Size msz = model->GetMapSizeInTiles();
    Grid g(msz.width, msz.height);
    for (int j = 0; j < msz.height; j++)
        for (int i = 0; i < msz.width; i++)
            g.SetAt(i, j, Map::IsCollidableTile(model->GetTileId(i, j)) ? 1 : 0);
    return MarkReachablePath(g, Location(start.x, start.y), Location(end.x, end.y));
}
