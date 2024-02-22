#ifndef _VERTEX_H
#define _VERTEX_H

#include "types/math.h"
#include "types/tuple3t.h"
#include "types/tuple2t.h"

class Vertex
{
public:

    enum { UV_COUNT = 1 };

    Tuple3T<float> position,
                   normal;
    Tuple2T<float> uv[UV_COUNT];

    inline Vertex()                                         {}
    inline Vertex(const Vertex &v)                          { memcpy(this, (const void *)&v, sizeof(Vertex)); }
    inline Vertex &operator=(const Vertex &v)               { if (this != &v) memcpy(this, (const void *)&v, sizeof(Vertex)); return *this; }

    inline const Tuple3T<float> &getposition() const
    {
        return position;
    }
    inline const Tuple3T<float> &getnormal() const
    {
        return normal;
    }
    inline const Tuple2T<float> &getuv(int index) const
    {
        return uv[index];
    }
    inline void setposition(const Tuple3T<float> &pos)
    {
        position = pos;
    }
    inline void setnormal(const Tuple3T<float> &norm)
    {
        normal = norm;
    }
    inline void setuv(int index, const Tuple2T<float> &tex)
    {
        uv[index] = tex;
    }

    static void interpolate(float t, const Vertex &a, const Vertex &b, Vertex &c)
    {
        if      (math::eq(t, 0.0f)) c = a;
        else if (math::eq(t, 1.0f)) c = b;
        else
        {
    	    c.position = a.position + (b.position - a.position) * t;
    	    c.normal   = a.normal   + (b.normal   - a.normal)   * t;
			for (int i = 0; i < UV_COUNT; i++)
    	        c.uv[i] = a.uv[i] + (b.uv[i] - a.uv[i]) * t;
        }
    }
    static inline void interpolate(const Tuple3T<float> &p, const Vertex &a, const Vertex &b, Vertex &c)
    {
        float range = a.position.distance(b.position);
        if (math::eq(range, 0.0f)) c = a;
        else interpolate(a.position.distance(p) / range, a, b, c);
    }

private:

    static inline unsigned int m_byte(float t, unsigned int u, unsigned int v, int sh)
    {
        u = (u >> sh) & 0x0ff; v = (v >> sh) & 0x0ff;
        return ((unsigned int)(u + (float)(v - u) * t) & 0xff) << sh;
    }
    static inline unsigned int m_color(float t, unsigned int a, unsigned int b)
    {
        return m_byte(t, a, b, 24) | m_byte(t, a, b, 16) | m_byte(t, a, b, 8) | m_byte(t, a, b, 0);
    }
};

#endif  // _VERTEX_H