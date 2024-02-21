#include "Collider.h"
#include "core/core.h"
#include "core/plane.h"
#include "core/file.h"

#include "core/core_assert.h"
#include "core/strs.h"

struct Node
{
    enum Type { POINTER = 0, OUT = -1, IN = -2, PARTITION = -3, _ = 0x7fffffff };

    Type   type;
    float  plane[4];
    Node  *neg,
          *pos;

    inline static bool IsType(const Node *n)
    {
        intptr_t ni = (intptr_t)n;
        return ni < 0 && ni >= PARTITION;
    }
    inline static Type GetType(const Node *n)
    {
        intptr_t ni = (intptr_t)n;
        return (ni < 0 && ni >= PARTITION) ? (Type)ni : POINTER;
    }
    inline static bool IsInOut(Type type)
    {
        return type == IN || type == OUT;
    }
};

struct Node32
{
    enum Type { POINTER = 0, OUT = -1, IN = -2, PARTITION = -3, _ = 0x7fffffff };

    int32_t type;
    float   plane[4];
    int32_t neg,
            pos;

    inline static bool IsType(const int32_t n)
    {
        int32_t ni = n;
        return ni < 0 && ni >= PARTITION;
    }
    inline static Type GetType(const int32_t n)
    {
        int32_t ni = n;
        return (ni < 0 && ni >= PARTITION) ? (Type)ni : POINTER;
    }
    inline static bool IsInOut(int32_t type)
    {
        return type == IN || type == OUT;
    }
    
    inline char *type_str(int32_t type)
    {
        char pointer[64];
        memset(pointer, 0, sizeof(pointer));
        const char *str = 0;
        switch (type) {
            case POINTER:
                str = "POINTER";
                break;
            case OUT:
                str = "OUT";
                break;
            case IN:
                str = "IN";
                break;
            case PARTITION:
                str = "PARTITION";
                break;
            case _:
                str = "_";
                break;
            default:
                sprintf(pointer, "%d", type);
                str = pointer;
                break;
        }
        return strs_strdup(str);
    }
    
    inline void print()
    {
        char *str = 0;
        str = type_str(this->type);
        printf("T: %s\n", str);
        free(str);
        printf("p: %.2f %.2f %.2f %.2f\n",
               this->plane[0], this->plane[1], this->plane[2], this->plane[3]
               );
        str = type_str(this->neg);
        printf("-: %s\n", str);
        free(str);
        str = type_str(this->pos);
        printf("+: %s\n", str);
        free(str);
        printf("\n");
    }
};

struct Tree32
{
    Node32 *nodes;
    int     count;
  
    inline void print()
    {
        for (int i = 0; i < this->count; i++)
        {
            (this->nodes)[i].print();
        }
    }
};

struct Tree64
{
    Node *nodes;
    int   count;
};

static Node::Type GetFullType(const Node *n)
{
    if (!n) return Node::OUT;
    Node::Type t = Node::GetType(n);
    if (t != Node::POINTER)
        return t;
    return n->type;
}

inline static int fsign(float f)
{
    if (math::lt_zero(f)) return -1;
    if (math::gt_zero(f)) return  1;
    return 0;
}

Tree64 ConvertTo64(Tree32 tree32)
{
    Tree64 tree64;
    tree64.count = tree32.count;
    tree64.nodes = new Node [tree32.count];
    memset((void *)tree64.nodes, 0, tree64.count * sizeof(Node));
    for (int i = 0; i < tree32.count; i++)
    {
        Node32 *n32 = &(tree32.nodes[i]);
        Node *n64 = &(tree64.nodes[i]);
        memcpy(n64->plane, n32->plane, sizeof(n32->plane));
        n64->type = (Node::Type)n32->type;
        if (n32->neg != Node32::IN && n32->neg != Node32::OUT &&
            n32->neg != Node32::PARTITION && n32->neg != Node32::_) {
            n64->neg = (Node *)(((intptr_t)(n32->neg) / sizeof(Node32)) * sizeof(Node));
        } else {
            n64->neg = (Node *)(intptr_t)n32->neg;
        }
        if (n32->pos != Node32::IN && n32->pos != Node32::OUT &&
            n32->pos != Node32::PARTITION && n32->pos != Node32::_) {
            n64->pos = (Node *)(((intptr_t)(n32->pos) / sizeof(Node32)) * sizeof(Node));
        } else {
            n64->pos = (Node *)(intptr_t)n32->pos;
        }
    }
    
    return tree64;
}

Node *ReadCollider(const char *filename)
{
    printf("Read collider '%s'!\n", filename);
    file::buffer_t buffer = file::read_file(filename);
    if (!buffer.data || !buffer.size)
    {
        printf("Unable to read collider '%s'!\n", filename);
        return 0;
    }
    Tree32 t32;
    t32.count = (int)buffer.size / sizeof(Node32);
    t32.nodes = (Node32 *)buffer.data;
    //t32.print();
    Tree64 t64  = ConvertTo64(t32);
    char   *raw = (char *)t64.nodes;
    // fixup
    for (int i = 0; i < t64.count; i++)
    {
        Node &n = t64.nodes[i];
        if (!Node::IsType(n.neg)) n.neg = (Node *)(raw + (intptr_t)n.neg);
        if (!Node::IsType(n.pos)) n.pos = (Node *)(raw + (intptr_t)n.pos);
        
    }
    delete [] t32.nodes;
    return t64.nodes;
}

Collider *Collider::ReadCollider(const char *filename)
{
    Node *tree = ::ReadCollider(filename);
    if (!tree)
        return 0;
    Collider *collider = new Collider;
    collider->mData = tree;
    return collider;
}

Collider::~Collider()
{
    if (mData)
    {
        delete [] (char *)mData;
        mData = 0;
    }
}

static Collider::Classification TypeToClassification(Node::Type type)
{
    switch (type)
    {
    case Node::OUT:       return Collider::CLASS_OUT;
    case Node::IN:        return Collider::CLASS_IN;
    case Node::PARTITION: return Collider::CLASS_ON;
    }
    return Collider::CLASS_ERROR;
}

static Collider::Classification ClassifyPoint(const Tuple3f &point, const Node *n)
{
    Node::Type type = GetFullType(n);
    if (Node::IsInOut(type))
        return TypeToClassification(type);
    Node *nn = n->neg, *np = n->pos;
    // this is the partition handling code
    const Plane &plane = *(const Plane *)n->plane;
    float distance = plane.distance(point);
    if      (math::lt_zero(distance)) return ClassifyPoint(point, nn);
    else if (math::gt_zero(distance)) return ClassifyPoint(point, np);
    // point is on plane, so classify the neighborhood of point by 
    // filtering the same point down both branches.  
    Collider::Classification cn = ClassifyPoint(point, nn),
                             cp = ClassifyPoint(point, np);
    // if classification is same then return it otherwise it's on 
    return (cn == cp) ? cn : Collider::CLASS_ON;
}

Collider::Classification Collider::ClassifyPoint(const Tuple3f &point) const
{
    return ::ClassifyPoint(point, (const Node *)mData);
}

static Collider::Classification ClassifySphere(const Tuple3f &point, float radius, const Node *n, Tuple3f &resolve)
{
    Node::Type type = GetFullType(n);
    if (Node::IsInOut(type))
        return TypeToClassification(type);
    Node *nn = n->neg, *np = n->pos;
    // this is the partition handling code
    const Plane &plane = *(const Plane *)n->plane;
    float distance = plane.distance(point);
    if      (math::lt(distance, -radius)) return ClassifySphere(point, radius, nn, resolve);
    else if (math::gt(distance,  radius)) return ClassifySphere(point, radius, np, resolve);
    // point is on plane, so classify the neighborhood of point by 
    // filtering the same point down both branches.
    Tuple3f resolven, resolvep;
    Collider::Classification cn = ClassifySphere(point, radius, nn, resolven),
                             cp = ClassifySphere(point, radius, np, resolvep);
    // if classification is same then return it otherwise it's on 
    resolve = -(distance - radius) * plane.normal; // this should point outward
    /*
    if (cn == Collider::CLASS_ON && cp == Collider::CLASS_ON)
    {
        float rsz = resolve.lengthsq(), rnsz = resolven.lengthsq(), rpsz = resolvep.lengthsq();
        if (rsz > rnsz)
        {
            resolve = resolven;
            rsz     = rnsz;
        }
        if (rsz > rpsz)
        {
            resolve = resolvep;
            rsz     = rpsz;
        }
    }
    else if (cn == cp)
        return cn;
    else if (cn == Collider::CLASS_ON)
        resolve = resolven;
    else if (cp == Collider::CLASS_ON)
        resolve = resolvep;
    */
    if (cn == cp)
        return cn;
    return Collider::CLASS_ON;
}
/*
static Collider::Classification ClassifyRadius(const Tuple3f &point, float radius, const Node *n, Tuple3f &minNegVector, Tuple3f &at)
{
    Node::Type type = GetFullType(n);
    /*
    if (type == Node::IN)
    {
        float distance = prevPlane.distance(point);
        at = -(distance - radius) * prevPlane.normal;
    }
    * /
    if (Node::IsInOut(type))
        return TypeToClassification(type);
    Node *nn = n->neg, *np = n->pos;
    // this is the partition handling code
    const Plane &plane = *(const Plane *)n->plane;
    float distance = plane.distance(point);
    if      (math::lt_zero(distance - radius))
    {
        Tuple3f localMinNegVec, resolve(-(distance - radius) * plane.normal);
        Collider::Classification c = ClassifyRadius(point, radius, nn, localMinNegVec, at);
        if (c != Collider::CLASS_OUT)
        {
            if (localMinNegVec.lengthsq() < minNegVec.lengthsq())
                minNegVec = localMinNegVec;
        }
        if (resolve.lengthsq() < minNegVec.lengthsq())
            minNegVec = resolve;
        return c;
    }
    else if (math::gt_zero(distance - radius))
    {

        Collider::Classification c = ClassifyRadius(point, radius, np, minNegVec, at);
//        if (c == Collider::CLASS_OUT || c == Collider::CLASS_ON)
//            at = -(distance - radius) * plane.normal;
        return c;
    }
    // point is on plane, so classify the neighborhood of point by 
    // filtering the same point down both branches.
    /// $TODO
    Tuple3f atn, atp;
    //at = (-distance * plane.normal) * point;
    Collider::Classification cn = ClassifyRadius(point, radius, nn, minNegVec, atn),
                             cp = ClassifyRadius(point, radius, np, minNegVec, atp);
    if (cn == Collider::CLASS_ON && cp == Collider::CLASS_ON)
    {
        at = atn; // $TODO for now, arbitrary
    }
    else if (cn == cp)
        return cn;
    else if (cn == Collider::CLASS_ON)
        at = atn; 
    else if (cp == Collider::CLASS_ON)
        at = atp; 
    return Collider::CLASS_ON;
}
*/

static Collider::Classification ClassifyRadius(const Tuple3f &point, float radius, const Node *n)
{
    Node::Type type = GetFullType(n);
    if (Node::IsInOut(type))
        return TypeToClassification(type);
    Node *nn = n->neg, *np = n->pos;
    // this is the partition handling code
    const Plane &plane = *(const Plane *)n->plane;
    float distance = plane.distance(point);
    if      (math::lt_zero(distance - radius)) return ClassifyRadius(point, radius, nn);
    else if (math::gt_zero(distance - radius)) return ClassifyRadius(point, radius, np);
    // point is on plane, so classify the neighborhood of point by 
    // filtering the same point down both branches.
    Collider::Classification cn = ClassifyRadius(point, radius, nn),
                             cp = ClassifyRadius(point, radius, np);
    if (cn == cp)
        return cn;
    return Collider::CLASS_ON;
}

Collider::Classification Collider::ClassifySphere(const Tuple3f &point, float radius, Tuple3f &resolve) const
{
    /*
    Tuple3f closest, at;
    Collider::Classification c = ::ClassifyRadius(point, radius, (const Node *)mData, closest);
    if (c == Collider::CLASS_OUT || c == Collider::CLASS_ON)
        return Collider::CLASS_OUT;
    if (c == Collider::CLASS_IN)
        c = ::ClassifyRadius(point, 0.0f, (const Node *)mData, at);
    resolve = -(closest - point);
    if (c == Collider::CLASS_OUT || c == Collider::CLASS_ON)
        c = Collider::CLASS_ON;
    return c;
    */
    //return ::ClassifySphere(point, radius, (const Node *)mData, resolve);
    return ::ClassifyRadius(point, radius, (const Node *)mData);
}

struct InteriorClassifier
{
    bool anyPieceOfLineIn, anyPieceOfLineOut;

    inline InteriorClassifier() : anyPieceOfLineIn(0), anyPieceOfLineOut(0) {}
};

static int LineIntersectsPlane(const Tuple3f &a, const Tuple3f &b, const Plane &plane, Tuple3f &at)
{
   int sign1 = 0, sign2 = 0; // must be int since gonna do a bitwise ^ 

   // get signs 
   sign1 = fsign(plane.distance(a));
   if (sign1 == 0)
       return 0; // we don't consider this an intersection

   sign2 = fsign(plane.distance(b));
   if (sign1 == 0)
   {
      at = b;
      return (sign1 < 0) ? -1 : 1;
   }

   // signs different? 
   // recall: -1^1 == 1^-1 ==> 1    case 4 & 5, see Gems III
   //         -1^-1 == 1^1 ==> 0    case 2 & 3, see Gems III
   if (sign1 ^ sign2)
   {
      float denom = 0.f, tt = 0.f;

      // compute intersection point 

      Tuple3f d(b - a);

      denom = plane.dot(d);//(aa * dx) + (bb * dy) + (cc * dz);
      core_assert(denom != 0.0f);
      tt = - plane.distance(a)/*((aa * x1) + (bb * y1) + (cc * z1) + dd)*/ / denom;

      at = Tuple3f(a + (d * tt));

      core_assert(sign1 != 0);

      return (sign1 < 0) ? -1 : 1;
   }

   return 0;
}

static void ClassifyLineInterior(const Tuple3f &from, const Tuple3f &to, const Node *n, InteriorClassifier &classifier)
{
    Node::Type type = GetFullType(n);
    if      (type == Node::IN)  { classifier.anyPieceOfLineIn  = true; return; }
    else if (type == Node::OUT) { classifier.anyPieceOfLineOut = true; return; }
    const Plane &plane = *(const Plane *)n->plane;
    float dp1 = plane.distance(from),
          dp2 = plane.distance(to);
    int   sign1 = fsign(dp1),
          sign2 = fsign(dp2);
    if ((sign1 < 0 && sign2 > 0) || (sign1 > 0 && sign2 < 0)) // split
    {
        Tuple3f intersection;

        int check = LineIntersectsPlane(from, to, plane, intersection);

        core_assert(check != 0);
         
        // filter split line segments down appropriate branches 
        if (sign1 < 0)
        {
            ClassifyLineInterior(from, intersection, n->neg, classifier);
            ClassifyLineInterior(to,   intersection, n->pos, classifier);
        }
        else
        {
            ClassifyLineInterior(from, intersection, n->pos, classifier);
            ClassifyLineInterior(to,   intersection, n->neg, classifier);
        }
    }
    else if (sign1 == 0 && sign2 == 0) // both on
    {
        ClassifyLineInterior(from, to, n->neg, classifier);
        ClassifyLineInterior(from, to, n->pos, classifier);
    }
    else if (sign1 < 0 || sign2 < 0) // both neg
        ClassifyLineInterior(from, to, n->neg, classifier);
    else  // both pos
        ClassifyLineInterior(from, to, n->pos, classifier);
}

static bool CollisionOccurred(const Tuple3f &from, const Tuple3f &to, const Node *n)
{
    if (from == to)
    {
        Collider::Classification cc = ClassifyPoint(from, n);
        return cc == Collider::CLASS_IN || cc == Collider::CLASS_ON;
    }

    // first classify the endpoints 
    Collider::Classification c1 = ClassifyPoint(from, n),
                             c2 = ClassifyPoint(to,   n);

    // collision occurs iff there's a state change between endpoints or
    // either endpoint is on an object 
    if (c1 == Collider::CLASS_ON || c2 == Collider::CLASS_ON || c1 != c2)
        return true;
    else
    {
        // since we already classified the endpoints, try interior of line 
        //    this routine will set the flags to appropriate values
        InteriorClassifier classifier;
        ClassifyLineInterior(from, to, n, classifier);
        // if line interior is inside and outside an object, collision detected
        // else no collision detected 
        return classifier.anyPieceOfLineIn && classifier.anyPieceOfLineOut;
    }
}

bool Collider::CollisionOccurred(const Tuple3f &from, const Tuple3f &to) const
{
    return ::CollisionOccurred(from, to, (const Node *)mData);
}
