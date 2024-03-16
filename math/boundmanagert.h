#ifndef _BOUNDMANAGERT_H
#define _BOUNDMANAGERT_H

#include "aaboxt.h"
#include "iboundchangelistener.h"

template <class NotifyRefT>
class BoundManagerT
{
    bool                    m_computed;
    AABoxT<float>           m_box;

    std::vector<NotifyRefT> m_approvers,
	                        m_listeners;

    inline bool m_clearApproved(const AABoxT<float> &from)
	{
        for (std::vector<NotifyRefT>::iterator i = m_approvers.begin(), e = m_approvers.end(); i != e; ++i)
		    if (!((IBoundChangeListener *)(*i))->boundClearApproved(from))
			    return false;
		return true;
	}
	inline bool m_setApproved(const AABoxT<float> &from, const AABoxT<float> &to)
	{
        for (std::vector<NotifyRefT>::iterator i = m_approvers.begin(), e = m_approvers.end(); i != e; ++i)
		    if (!((IBoundChangeListener *)(*i))->boundSetApproved(from, to))
			    return false;
		return true;
	}
	inline void m_clear(const AABoxT<float> &from)
	{
        for (std::vector<NotifyRefT>::iterator i = m_approvers.begin(), e = m_approvers.end(); i != e; ++i)
		    ((IBoundChangeListener *)(*i))->boundClear(from);
	}
	inline void m_set(const AABoxT<float> &from, const AABoxT<float> &to)
	{
        for (std::vector<NotifyRefT>::iterator i = m_approvers.begin(), e = m_approvers.end(); i != e; ++i)
		    ((IBoundChangeListener *)(*i))->boundSet(from, to);
	}

public:

    inline BoundManagerT() : m_computed(false) {}

    inline bool                  addApprover(NotifyRefT r)              { m_approvers.push_back(r); }
    inline bool                  addListener(NotifyRefT r)              { m_listeners.push_back(r); }

    inline bool                  computed(void) const                   { return m_computed; }
    inline bool                  clear(void)
	{
	    if (m_computed && m_clearApproved(m_box))
		{
		    m_clear(m_box);
	        m_computed = false;
			m_box = AABoxT<float>();
			return true;
		}
		return false;
	}
    inline const AABoxT<float> &getAABox(void) const                    { return m_box; }
    inline bool                 setAABox(const AABoxT<float> &b)
	{
	    if (m_box != b && m_setApproved(m_box, b))
		{
		    m_set(m_box, b);
			m_computed = true;
		    m_box = b;
			return true;
		}
		return false;
	}
};

#endif  /** _BOUNDMANAGERT_H */
