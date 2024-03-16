#ifndef _INDEXLIST_H
#define _INDEXLIST_H

#include <vector>

class IndexList
{
public:

    std::vector<int> list;

	inline void   add(int id)	          { list.push_back(id); }
	inline int    pop(void)               { int v = list.back(); list.pop_back(); return v; }
	inline int    top(void)         const { return list.back(); }
	inline bool   empty(void)       const { return list.empty(); }
	inline size_t size(void)        const { return list.size(); }
	inline void   clear(void)             { list.clear(); }

	inline int    operator[](int i) const { return list[i]; }
};

#endif  /** _INDEXLIST_H */
