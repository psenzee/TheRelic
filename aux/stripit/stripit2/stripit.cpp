#include <string.h>
#include <vector>
#include <algorithm>

#include <conio.h>
#include <assert.h>

struct Vertex
{
    // ..
};

struct Triangle
{
    int  id[3];
    int  refs;
};

class TriList // this must be immutable once created, because we maintain pointers to the triangles inside
{
public:

    TriList(const Triangle *tris, int count)
    {
        mTriangles = new Triangle [count];
        mCount     = count;
        memcpy(mTriangles, tris, mCount * sizeof(Triangle));
    }
    ~TriList()
    {
        delete [] mTriangles;
        mTriangles = 0;
        mCount = 0;
    }
    inline Triangle &GetTriangle(int id)       { return mTriangles[id]; }
    inline int       GetCount()          const { return mCount; }
    
private:

    Triangle *mTriangles;
    int       mCount;
};

typedef std::vector<Triangle *> TriRefList;
typedef std::vector<Vertex>     VertexList;
typedef std::vector<int>        IndexList;

class TriMap
{
public:

    enum { MAX_INDICES = 65536 };

    TriMap(const IndexList &ids);
    ~TriMap();
    
    Triangle **GetAdjacent(Triangle *t, int a, int b);

    TriList *GetList() { return mList; }

    void Print()    
    {
        int indexCount = 0;
        int neighbors[20];
        memset(neighbors, 0, sizeof(neighbors));
        for (int i = 0; i < MAX_INDICES; i++)
        {
            if (mMap[i] && !mMap[i]->empty())
            {
                indexCount++;
                size_t n = mMap[i]->size();
                if (n < 20)
                    neighbors[n]++;
            }
        }
        printf("Neighbors ");
        for (int i = 0; i < (sizeof(neighbors) / sizeof(int)); i++)
        {
            if (neighbors[i])
                printf("%d=%d; ", i, neighbors[i]);
        }
        printf("\n");
    }

private:

    inline TriRefList &GetOrCreateList(int id);
    inline bool        HasTriangles(int id) const
    { 
        return (id >= 0 && id < MAX_INDICES && mMap) ? !mMap[id]->empty() : false;
    }
    void               Make(TriList &triangles);
    void               Intersection(const TriRefList &a, const TriRefList &b);
    
    static TriList    *MakeTriList(const IndexList &ids);    

    TriList           *mList;
    TriRefList        *mMap[MAX_INDICES];
    int                mMaxListSize;
    Triangle         **mAdjacentList;
};

void TriMap::Intersection(const TriRefList &a, const TriRefList &b)
{
    memset(mAdjacentList, 0, sizeof(Triangle *) * (mMaxListSize + 1));
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), mAdjacentList);
}

Triangle **TriMap::GetAdjacent(Triangle *t, int a, int b)
{
    a = t->id[a]; b = t->id[b];
    if (!HasTriangles(a) || !HasTriangles(b))
        return false;
    memset(mAdjacentList, 0, sizeof(Triangle *) * (mMaxListSize + 1));
    Intersection(*mMap[a], *mMap[b]);
    return mAdjacentList;
}

TriList *TriMap::MakeTriList(const IndexList &ids)
{
    std::vector<Triangle> triangles;
    for (size_t i = 2, sz = ids.size(); i < sz; i += 3)
    {
        Triangle t;
        t.refs  = 0;
        t.id[0] = ids[i - 2];
        t.id[1] = ids[i - 1];
        t.id[2] = ids[i - 0];
        triangles.push_back(t);
    }
    return new TriList(&triangles[0], static_cast<int>(triangles.size()));
}

inline TriRefList &TriMap::GetOrCreateList(int id)
{
    TriRefList *list = mMap[id];
    if (!list)
        mMap[id] = list = new TriRefList;
    return *list;
}

TriMap::TriMap(const IndexList &ids) : mList(0), mMaxListSize(0), mAdjacentList(0)
{
    mList = MakeTriList(ids);
    memset(mMap, 0, sizeof(mMap));
    Make(*mList);
}

TriMap::~TriMap()
{
    memset(mMap, 0, sizeof(mMap));
    delete mList;
}

void TriMap::Make(TriList &tris)
{
    for (int i = 0, count = tris.GetCount(); i != count; i++)
    {
        Triangle *tri = &tris.GetTriangle(i);
        GetOrCreateList(tri->id[0]).push_back(tri);
        GetOrCreateList(tri->id[1]).push_back(tri);
        GetOrCreateList(tri->id[2]).push_back(tri);
    }
    mMaxListSize = 0;
    for (int i = 0; i != MAX_INDICES; i++)
    {
        if (mMap[i] && !mMap[i]->empty())
        {
            if (mMap[i]->size() > mMaxListSize)
                mMaxListSize = static_cast<int>(mMap[i]->size());
            std::sort(mMap[i]->begin(), mMap[i]->end());
        }
    }
    mAdjacentList = new Triangle * [mMaxListSize + 1];
    memset(mAdjacentList, 0, sizeof(Triangle *) * (mMaxListSize + 1));
}

int indices[] = 
{
    32,  29,  31,
    29,  32,  28,
    28,  13,  10,
    28,  32,  13,
    33,  31,  29,
    12,  33,  11,
    30,  11,  33,
    30,  33,  29,
    34,  28,  35,
    28,  34,  29,
    35,  10,  14,
    10,  35,  28,
    15,  30,  36,
    30,  15,  11,
    36,  29,  34,
    29,  36,  30,
    37,  35,  38,
    35,  37,  34,
    38,  14,  16,
    14,  38,  35,
    17,  36,  39,
    36,  17,  15,
    39,  34,  37,
    34,  39,  36,
    18,  43,  40,
    18,  40,  42,
    31,  41,  44,
    40,  19,  44,
    44,  32,  31,
    48,  21,  32,
    13,  32,  21,
    19,  40,  43,
    18,  42,  12,
    48,  19,  21,
    33,  12,  42,
    42,  41,  33,
    33,  41,  31,
    27,  22,  47,
    74,  46,  22,
    46,  47,  22,
    20,  46,  23,
    25,  45,  55,
    18,  25,  43,
    24,  45,  26,
    25,  26,  45,
    44,  19,  48,
    44,  48,  32,
    46,  49,  47,
    46,  20,  49,
    37,  38,  51,
    52,  51,  38,
    39,  37,  51,
    52,  38,  50,
    50,  38,  16,
    17,  39,  50,
    51,  50,  39,
    50,  16,  17,
    55,  45,  53,
    49,  53,  47,
    53,  45,  54,
    54,  47,  53,
    54,  45,  27,
    54,  27,  47,
    24,  27,  45,
    25,  55,  43,
    19,  43,  55,
    55,  53,  49,
    55,  49,  19,
    19,  49,  20,
    60,  59,  57,
    57,  56,  60,
    56,  10,  13,
    56,  13,  60,
    61,  57,  59,
    12,  11,  61,
    58,  61,  11,
    58,  57,  61,
    62,  63,  56,
    56,  57,  62,
    63,  14,  10,
    10,  56,  63,
    15,  64,  58,
    58,  11,  15,
    64,  62,  57,
    57,  58,  64,
    65,  66,  63,
    63,  62,  65,
    66,  16,  14,
    14,  63,  66,
    17,  67,  64,
    64,  15,  17,
    67,  65,  62,
    62,  64,  67,
    18,  68,  71,
    18,  70,  68,
    59,  72,  69,
    68,  72,  19,
    72,  59,  60,
    76,  60,  21,
    13,  21,  60,
    19,  71,  68,
    18,  12,  70,
    76,  21,  19,
    61,  70,  12,
    70,  61,  69,
    61,  59,  69,
    27,  75,  22,
    23,  46,  74,
    74,  22,  75,
    20,  23,  74,
    25,  83,  73,
    18,  71,  25,
    24,  26,  73,
    25,  73,  26,
    72,  76,  19,
    72,  60,  76,
    74,  75,  77,
    74,  77,  20,
    65,  79,  66,
    80,  66,  79,
    67,  79,  65,
    80,  78,  66,
    78,  16,  66,
    17,  78,  67,
    79,  67,  78,
    78,  17,  16,
    83,  81,  73,
    77,  75,  81,
    81,  82,  73,
    82,  81,  75,
    73,  27,  24,
    82,  75,  27,
    27,  73,  82,
    25,  71,  83,
    19,  83,  71,
    83,  77,  81,
    83,  19,  77,
    19,  20,  77,
    41,  84,  44,
    86,  42,  40,
    84,  41,  42,
    44,  87,  40,
    87,  44,  84,
    87,  86,  40,
    85,  42,  86,
    42,  85,  84,
    88,  84,  89,
    86,  93,  89,
    87,  90,  86,
    90,  87,  84,
    84,  92,  90,
    91,  89,  84,
    86,  89,  91,
    86,  91,  85,
    84,  85,  91,
    89,  93,  88,
    88,  93,  92,
    86,  90,  93,
    84,  88,  92,
    69,  72,  94,
    94,  70,  69,
    97,  94,  72,
    95,  96,  70,
    70,  94,  95,
    98,  99,  94,
    96,  99, 103,
    97,  96, 100,
    101,  94,  99,
    94, 101,  95,
    99,  98, 103,
    98, 102, 103,
    96,  68,  70,
    72,  68,  97,
    97,  68,  96,
    100,  94,  97,
    94, 100, 102,
    96, 101,  99,
    96,  95, 101,
    96, 103, 100,
    94, 102,  98,
    50,  51, 104,
    104,  51, 105,
    50, 104, 106,
    105,  51, 107,
    106, 105, 107,
    105, 106, 104,
    51,  52, 107,
    52, 106, 107,
    52,  50, 106,
    78, 108,  79,
    108, 109,  79,
    78, 110, 108,
    109, 111,  79,
    110, 111, 109,
    109, 108, 110,
    79, 111,  80,
    80, 111, 110,
    80, 110,  78,
    78, 108,  79,
    108, 109,  79,
    78, 110, 108,
    109, 111,  79,
    110, 111, 109,
    109, 108, 110,
    79, 111,  80,
    80, 111, 110,
    80, 110,  78,
    112, 113, 114,
    90, 113, 112,
    114,  90,  92,
    112, 116, 115,
    92, 117, 114,
    118, 112, 115,
    116, 112, 114,
    115, 117, 118,
    90, 112,  93,
    117,  92,  93,
    115, 116, 114,
    113,  90, 114,
    117, 115, 114,
    118, 117,  93,
    93, 112, 118,
    119, 121, 120,
    100, 119, 120,
    121, 102, 100,
    119, 122, 123,
    102, 121, 124,
    125, 122, 119,
    123, 121, 119,
    122, 125, 124,
    100, 103, 119,
    124, 103, 102,
    122, 121, 123,
    120, 121, 100,
    124, 121, 122,
    125, 103, 124,
    103, 125, 119,
    -1
};

void CopyIndices(const int *indices, std::vector<int> &into)
{
    for (int i = 0; indices[i] != -1; i++)
        into.push_back(indices[i]);
}

void RemoveRefs(std::vector<Triangle *> &list)
{               
    for (std::vector<Triangle *>::iterator i = list.begin(),
                                           e = list.end(); i != e; ++i)
       (*i)->refs--;
}

void GreedyStrip(TriMap &map, Triangle *tri, std::vector<Triangle *> &longest)
{
    const int edgeStart[] = { 0, 1, 2 }, edgeEnd[] = { 1, 2, 0 };
    if (!tri->refs)
    {
        tri->refs++;
        longest.push_back(tri);
        std::vector<Triangle *> remainingLongest, current;
        for (int edge = 0; edge < 3; edge++)
        {
            Triangle **triangles = 0;
            if ((triangles = map.GetAdjacent(tri, edgeStart[edge], edgeEnd[edge])) != 0)
            {
                for (int i = 0; triangles[i]; ++i)
                {
                    if (!triangles[i]->refs)
                    {
                        current.clear();
                        GreedyStrip(map, triangles[i], current);
                        if (current.size() > remainingLongest.size())
                            remainingLongest = current;
                        else
                            RemoveRefs(current);
                    }
                }
            }
        }
        if (!remainingLongest.empty())
            std::copy(remainingLongest.begin(), remainingLongest.end(), std::back_inserter(longest));
    }
}

Triangle *GetUnmarkedTriangle(TriMap &map)
{
    int count = map.GetList()->GetCount();
    for (int i = 0; i < count; i++)
    {
        Triangle *t = &(map.GetList()->GetTriangle(i));
        if (t && t->refs == 0)
            return t;
    }
    return 0;
}

void GetLongestStrip(TriMap &map, std::vector<Triangle *> &list)
{
    Triangle *t = GetUnmarkedTriangle(map);
    std::vector<Triangle *> longest;
    while (t)
    {
        t->refs++;
        std::vector<Triangle *> current;
        GreedyStrip(map, t, current);
        if (current.size() > longest.size())
            longest = current;
        else
            RemoveRefs(current);
        current.clear();
        t = GetUnmarkedTriangle(map);
        t->refs--;
    }
    list = longest;
}

void GetLongestStripPossible(TriMap &map, std::vector<Triangle *> &list)
{
    int count = map.GetList()->GetCount();
    std::vector<Triangle *> current;
    std::vector<Triangle *> longest;
    for (int i = 0; i < count; i++)
    {
        Triangle *t = &(map.GetList()->GetTriangle(i));
        if (!t->refs)
        {
            GreedyStrip(map, t, current);
            if (current.size() > longest.size())
                longest = current;
            else
                RemoveRefs(current);
            current.clear();
        }
    }
    list = longest;
}

void StripAll(TriMap &map, std::vector< std::vector<Triangle *> > &strips)
{
    Triangle *t = GetUnmarkedTriangle(map);
    while (t)
    {
        std::vector<Triangle *> list;
        GetLongestStripPossible(map, list);
        strips.push_back(list);
        printf("list: %d ", list.size());
        t = GetUnmarkedTriangle(map);
    }
}

void main()
{
    IndexList list;
    CopyIndices(indices, list);
    TriMap map(list);
    std::vector< std::vector<Triangle *> > strips;
    StripAll(map, strips);
    // verify that we didn't lose any tris
    size_t sum = 0;
    for (std::vector< std::vector<Triangle *> >::const_iterator i = strips.begin(), e = strips.end(); i != e; ++i)
        sum += (*i).size();
    printf("totals %d, stripped %d\n", map.GetList()->GetCount(), sum);
//    assert(map.GetList()->GetCount() == sum);
    for (std::vector< std::vector<Triangle *> >::const_iterator i = strips.begin(), e = strips.end(); i != e; ++i)
        for (std::vector<Triangle *>::const_iterator ji = (*i).begin(), je = (*i).end(); ji != je; ++ji)
            if ((*ji)->refs != 1)
                printf("bad refs value! %d; \n", (*ji)->refs);
    map.Print();
    getch();
}