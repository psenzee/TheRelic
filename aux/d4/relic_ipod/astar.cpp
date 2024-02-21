//..

#include <vector>
#include <queue>
#include <map>

struct Node
{
    const void  *data;
    float        gScore,
                 hScore,
                 fScore;
    Node        *cameFrom;
    bool         inClosedSet,
                 inOpenSet;

    inline Node(const void *data) : data(data), gScore(0.f), hScore(0.f), fScore(0.f), inClosedSet(false), inOpenSet(false), cameFrom(0) {}
    inline Node(const Node &n) : data(n.data), gScore(n.gScore), hScore(n.hScore), fScore(n.fScore), inClosedSet(n.inClosedSet), inOpenSet(n.inOpenSet), cameFrom(0) {}

    inline bool operator<(const Node &other) const { return fScore < other.fScore; }
    inline bool equals(const Node &other)    const { return data == other.data; }
};

class AStar
{
public:

    enum { MAX_NEIGHBORS = 1024 };

    typedef float (*distance_fn) (const void *a, const void *b);
    typedef int   (*neighbors_fn)(const void *a, void *neighbors[]);

    inline AStar() : mStart(0), mGoal(0) {}
    inline ~AStar() { Clear(); }

    void SetOptimisticEstimator(distance_fn dfn) { mOptimisticDistanceEstimator = dfn; }
    void SetDistanceFunction(distance_fn dfn)    { mDistanceFunction = dfn; }
    void SetNeighborsFunction(neighbors_fn nfn)  { mNeighborsFunction = nfn; }

    bool FindPath(const void *start, const void *goal, std::vector<const void *> &path);

private:

    enum PathResult { FOUND, NOT_FOUND, CONTINUE };

    struct Comparator
    {
        inline bool operator()(Node *a, Node *b) { return a->fScore < b->fScore; }
    };

    std::vector<Node *>                      mClosedSet; // % The set of nodes already evaluated.
    std::priority_queue<Node *,
        std::vector<Node *>, Comparator>     mOpenSet;   // % The set of tentative nodes to be evaluated.
    Node                                    *mStart,
                                            *mGoal;
    distance_fn                              mOptimisticDistanceEstimator,
                                             mDistanceFunction;
    neighbors_fn                             mNeighborsFunction;
    std::map<const void *, Node *>           mNodes;

    inline Node *NewNode(const void *data)
    {
        Node *n = new Node(data);
        mNodes[data] = n;
        return n;
    }

    void Clear()
    {
        mClosedSet.clear();
        while (!mOpenSet.empty())
            mOpenSet.pop(); // why isn't there a clear() on priority_queue?
        for (std::map<const void *, Node *>::iterator i = mNodes.begin(), e = mNodes.end(); i != e; ++i)
            delete (*i).second;
        mNodes.clear();
    }

    inline float OptimisticDistanceEstimate(const Node &a, const Node &b)
    {
        return mOptimisticDistanceEstimator(a.data, b.data);
    }

    inline float Distance(const Node &a, const Node &b)
    {
        return mDistanceFunction(a.data, b.data);
    }

    inline void PushClosedSet(Node *a)
    {
        if (!a->inClosedSet)
        {
            a->inClosedSet = true;
            mClosedSet.push_back(a);
        }
    }

    inline void PushOpenSet(Node *a)
    {
        if (!a->inOpenSet)
        {
            a->inOpenSet = true;
            mOpenSet.push(a);
        }
    }

    inline Node *PopOpenSet()
    {
        Node *x = mOpenSet.top();
        x->inOpenSet = false;
        mOpenSet.pop();
        return x;
    }

    void GetNeighbors(const Node &n, std::vector<Node *> &neighbors)
    {
        void *narray[MAX_NEIGHBORS];
        memset(narray, 0, sizeof(narray));
        int count = mNeighborsFunction(n.data, narray);
        neighbors.clear();
        for (int i = 0; i < count; i++)
            neighbors.push_back(NewNode(narray[i]));
    }

    void Start(const void *start, const void *goal)
    {
        mStart = NewNode(start);
        mGoal  = NewNode(goal);
        mStart->gScore = 0.f;
        mStart->hScore = OptimisticDistanceEstimate(*mStart, *mGoal);
        mStart->fScore = mStart->hScore;
        mClosedSet.clear();
        PushOpenSet(mStart);
    }

    PathResult Update()
    {
        if (mOpenSet.empty())
            return NOT_FOUND;

        Node *x = PopOpenSet();
        if (x->equals(*mGoal))
        {
            return FOUND;
        }

        PushClosedSet(x);

        std::vector<Node *> neighbors;
        GetNeighbors(x, neighbors);
        for (std::vector<Node *>::iterator i = neighbors.begin(), e = neighbors.end(); i != e; ++i)
        {
            if ((*i)->inClosedSet)
                continue;

            float gScore = x->gScore + Distance(x, *(*i));
            bool  better = false;
            if (!(*i)->inOpenSet)
            {
                PushOpenSet(*i);
                (*i)->hScore = OptimisticDistanceEstimate(*(*i), *mGoal);
                better = true;
            }
            else if (gScore < (*i)->gScore)
            {
                better = true;
            }
            if (better)
            {
                (*i)->cameFrom = x;
                (*i)->gScore   = gScore;
                (*i)->fScore   = gScore + (*i)->hScore;
            }
        }

        return CONTINUE;
    }

    void ReconstructPath(Node *n, std::vector<const void *> &path)
    {
        path.clear();
        while (n)
        {
            path.push_back(n->data);
            n = n->cameFrom;
        }
        std::reverse(path.begin(), path.end());
    }
};

bool AStar::FindPath(const void *start, const void *goal, std::vector<const void *> &path)
{
    Start(start, goal);
    PathResult result = CONTINUE;
    while (result == CONTINUE)
        result = Update();
    if (result == FOUND)
    {
        ReconstructPath(mGoal, path);
        Clear();
        return true;
    }
    path.clear();
    Clear();
    return false;
}

/*

 function A*(start,goal)
     closedset := the empty set                 % The set of nodes already evaluated.
     openset := set containing the initial node % The set of tentative nodes to be evaluated.
     g_score[start] := 0                        % Distance from start along optimal path.
     h_score[start] := heuristic_estimate_of_distance(start, goal)
     f_score[start] := h_score[start]           % Estimated total distance from start to goal through y.
     while openset is not empty
         x := the node in openset having the lowest f_score[] value
         if x = goal
             return reconstruct_path(came_from,goal)
         remove x from openset
         add x to closedset
         foreach y in neighbor_nodes(x)
             if y in closedset
                 continue
             tentative_g_score := g_score[x] + dist_between(x,y)
             tentative_is_better := false
             if y not in openset
                 add y to openset
                 h_score[y] := heuristic_estimate_of_distance(y, goal)
                 tentative_is_better := true
             elseif tentative_g_score < g_score[y]
                 tentative_is_better := true
             if tentative_is_better = true
                 came_from[y] := x
                 g_score[y] := tentative_g_score
                 f_score[y] := g_score[y] + h_score[y]
     return failure
 
 function reconstruct_path(came_from,current_node)
     if came_from[current_node] is set
         p = reconstruct_path(came_from,came_from[current_node])
         return (p + current_node)
     else
         return the empty path


*/

//http://en.wikipedia.org/wiki/A*_algorithm

void main()
{
    AStar as;
}