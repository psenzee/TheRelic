#ifndef _IBOUNDCHANGELISTENER_H
#define _IBOUNDCHANGELISTENER_H

#include "aaboxt.h"

class IBoundChangeListener
{
public:
    virtual ~IBoundChangeListener() {}

	virtual bool boundSetApproved(const AABoxT<float> &from, const AABoxT<float> &to) const = 0;
	virtual bool boundClearApproved(const AABoxT<float> &from)                        const = 0;
	virtual void boundSet(const AABoxT<float> &from, const AABoxT<float> &to)               = 0;
	virtual void boundClear(const AABoxT<float> &from)                                      = 0;
};

#endif  /** _IBOUNDCHANGELISTENER_H */
