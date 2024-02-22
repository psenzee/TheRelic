#ifndef _TIMER_H
#define _TIMER_H

#include <iostream>

class Timer
{
    int m_time;

public:

    Timer(void);

	void reset(void);
	int  time(void);
    void report(std::ostream &s, const char *message, int width = 32);
};

#endif  /** _TIMER_H */