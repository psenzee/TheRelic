#ifndef _TRANSFORMSTACKT_H
#define _TRANSFORMSTACKT_H

#include "math/transformt.h"
#include "math/matrix4t.h"
#include "math/tuple3t.h"
#include "math/affineelementt.h"

#include <vector>

template <typename T>
class TransformStackAuditorT
{
    typedef Matrix4T<T>            matrix_t;
    typedef Tuple3T<T>             vector_t;
    typedef AffineElementT<T>      element_t;
    typedef std::vector<element_t> element_list_t;

    element_list_t m_elements;

    void m_create(matrix_t &matrix, matrix_t &inverse) const
    {
        matrix.identity();
        for (element_list_t::const_iterator i = m_elements.begin(), e = m_elements.end(); i != e; ++i)
            (*i).apply(matrix, false);
        inverse.identity();
        for (element_list_t::const_reverse_iterator i = m_elements.rbegin(), e = m_elements.rend(); i != e; ++i)
            (*i).apply(inverse, true);
    }
    bool m_verify(const char *type, const matrix_t &auditor, const matrix_t &other)
    {
        if (auditor != other)
        {
            std::cerr << "WARNING: TransformStackAuditorT found inconsistency in '" << type << "': " << std::endl
                      << "  auditor:" << std::endl << auditor << std::endl << "  other:" << std::endl << other << std::endl;
            return false;
        }
        return true;
    }

public:
    inline matrix_t           matrix()                     const { matrix_t m, i; m_create(m, i); return m; }
    inline matrix_t           inverse()                    const { matrix_t m, i; m_create(m, i); return i; }

    inline void               pop()                              { m_elements.pop_back(); }

    inline void               translate(const vector_t &u)       { m_elements.push_back(element_t(element_t::TRANSLATE, u)); }
    inline void               scale(const vector_t &u)           { m_elements.push_back(element_t(element_t::SCALE,     u)); }
    inline void               rotate(const vector_t &u)          { m_elements.push_back(element_t(element_t::ROTATE,    u)); }

    bool                      verify(const matrix_t &m, const matrix_t &i)
    {
        matrix_t tm, ti; m_create(tm, ti);
        return m_verify("matrix", tm, m) && m_verify("inverse", ti, i);
    }
};

#define AUDITOR            TransformStackAuditorT<T> _auditor; inline bool audit_verify() { _auditor.verify(matrix(), inverse()); }
#define AUDIT_SCALE(u)     _auditor.scale(u);
#define AUDIT_ROTATE(u)    _auditor.rotate(u);
#define AUDIT_TRANSLATE(u) _auditor.translate(u);
#define AUDIT_ELEMENT(u)   { switch (u.type) { default: break; case AffineElementT<float>::SCALE: AUDIT_SCALE(u.vector) break; case AffineElementT<float>::ROTATE: AUDIT_ROTATE(u.vector) break; case AffineElementT<float>::TRANSLATE: AUDIT_TRANSLATE(u.vector) break; } }
#define AUDIT_VERIFY       audit_verify();

template <class T>
class TransformStackT
{
public:

    AUDITOR

    typedef Matrix4T<T>   matrix_t;
    typedef TransformT<T> transform_t;
    typedef Tuple3T<T>    vector_t;

    inline TransformStackT()                                             { transform_t t; t.identity(); m_transforms.push_back(t); }

    inline const matrix_t    &matrix()                             const { return m_transforms.back().matrix; }
    inline const matrix_t    &inverse()                            const { return m_transforms.back().inverse; }

    inline const transform_t &top()                                const { return m_transforms.back(); }
    inline void               pop()                                      { if (m_transforms.size() > 1) m_transforms.pop_back(); }

    inline void               translate(const vector_t &u)               { AUDIT_TRANSLATE(u) transform_t t; t.translate(u); push(t); AUDIT_VERIFY }
    inline void               scale(const vector_t &u)                   { AUDIT_SCALE(u)     transform_t t; t.scale(u);     push(t); AUDIT_VERIFY }
    inline void               rotate(const vector_t &u)                  { AUDIT_ROTATE(u)    transform_t t; t.rotate(u);    push(t); AUDIT_VERIFY }

    void                      push(const AffineElementT<float> &u)       { AUDIT_ELEMENT(u) transform_t xf; u.applyto(xf); push(xf); }

private:
  //inline void               push(const transform_t &t)                 { transform_t tr = m_transforms.back(); tr *= t; m_transforms.push_back(tr); }
    inline void               push(const transform_t &t)                 { transform_t tr = m_transforms.back(); transform_t u = t * tr; m_transforms.push_back(u); }
    std::vector<transform_t> m_transforms;
};

#endif  /** _TRANSFORMSTACKT_H */
