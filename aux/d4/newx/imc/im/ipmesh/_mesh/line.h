#ifndef _LINE_H
#define _LINE_H

#include <iostream>

class Line
{
public:

    int pa, pb;

	inline Line(int a = 0, int b = 0) : pa(a), pb(b) {}

    inline void set(int a, int b)                 { pa = a; pb = b; }
	inline __int64 id(void) const                 { return ((__int64)(pa < pb ? pb : pa) << 32) | (pa < pb ? pa : pb); }
	inline bool operator<(const Line &line) const { return id() < line.id(); }
	inline std::ostream &print(std::ostream &s)   { return s << "[line pa:" << pa << " pb:" << pb << "]"; }

	inline int getCommonPoint(const Line &ln)
	{
        return (pa == ln.pa || pa == ln.pb) ? pa : ((pb == ln.pb || pb == ln.pa) ? pb : -1);
	}
};

#endif  /** _LINE_H */
