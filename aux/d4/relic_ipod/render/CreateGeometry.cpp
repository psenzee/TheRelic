#include "CreateGeometry.h"
#include "core/core.h"
#include "core/mathcore.h"

#include <algorithm>

static inline void SetVertex(Vertex *p, float x, float y, float z, float u, float v)
{
    p->x = x; p->y = y; p->z = z; p->u = u; p->v = v;
}

// number of vertices written will be segments * 6
bool CreateRing2d(float radius0, float radius1, int segments, Vertex *vertices)
{
    if (radius0 == radius1)
        return false; // nothing to do..
    else if (radius0 > radius1)
        std::swap(radius0, radius1);
    else if (radius0 <= 0.f)
        // degenerate..
        return false;    
    float segmentAngle = math::TWOPIf / segments;
    Vertex *pv = vertices;
    for (int i = 0; i < segments; i++)
    {
        float angle0 = i * segmentAngle,      angle1 = angle0 + segmentAngle,
              sin0   = sin(angle0),           cos0   = cos(angle0),
              sin1   = sin(angle1),           cos1   = cos(angle1),
              u0     = angle0 / math::TWOPIf, u1     = angle1 / math::TWOPIf;
        
        // two triangles..
        SetVertex(pv++, sin1 * radius1, cos1 * radius1, 0.f, u1, 0.f); // (2)
        SetVertex(pv++, sin1 * radius0, cos1 * radius0, 0.f, u1, 1.f); // (1)
        SetVertex(pv++, sin0 * radius0, cos0 * radius0, 0.f, u0, 1.f); // (0)

        SetVertex(pv++, sin0 * radius0, cos0 * radius0, 0.f, u0, 1.f); // (0)
        SetVertex(pv++, sin0 * radius1, cos0 * radius1, 0.f, u0, 0.f); // (3)
        SetVertex(pv++, sin1 * radius1, cos1 * radius1, 0.f, u1, 0.f); // (2)
    }
    return true;
}

 // number of vertices written will be segments * 4, number of indices will be segments * 6
bool CreateRing2d(float radius0, float radius1, int segments, Vertex *vertices, unsigned short *indices)
{
    if (radius0 == radius1)
        return false; // nothing to do..
    else if (radius0 > radius1)
        std::swap(radius0, radius1);
    else if (radius0 <= 0.f)
        // degenerate..
        return false;    
    float segmentAngle = math::TWOPIf / segments;
    Vertex         *pv = vertices;
    unsigned short *pi = indices;
    for (int i = 0; i < segments; i++)
    {
        float angle0 = i * segmentAngle,      angle1 = angle0 + segmentAngle,
              sin0   = sin(angle0),           cos0   = cos(angle0),
              sin1   = sin(angle1),           cos1   = cos(angle1),
              u0     = angle0 / math::TWOPIf, u1     = angle1 / math::TWOPIf;
        
        // two triangles..
        SetVertex(pv++, sin0 * radius0, cos0 * radius0, 0.f, u0, 1.f); // (0)
        SetVertex(pv++, sin1 * radius0, cos1 * radius0, 0.f, u1, 1.f); // (1)
        SetVertex(pv++, sin1 * radius1, cos1 * radius1, 0.f, u1, 0.f); // (2)
        SetVertex(pv++, sin0 * radius1, cos0 * radius1, 0.f, u0, 0.f); // (3)
        
        int base = i * 4;
        *pi++ = 0 + base; *pi++ = 1 + base; *pi++ = 2 + base;
        *pi++ = 2 + base; *pi++ = 3 + base; *pi++ = 0 + base;
    }
    return true;
}

void Transform(Vertex *data, int count, Matrix &transform)
{
    for (int i = 0; i < count; i++)
    {
        Vertex &v = data[i];
        Vector3 p(v.x, v.y, v.z);
        p = transform * p;
        v.x = p.x; v.y = p.y; v.z = p.z;
    }
}