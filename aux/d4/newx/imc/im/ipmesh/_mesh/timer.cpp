#include "timer.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

Timer::Timer(void) : m_time(::timeGetTime())
{
}

void Timer::reset(void)
{
    m_time = ::timeGetTime();
}

int  Timer::time(void)
{
    return ::timeGetTime() - m_time;
}

void Timer::report(std::ostream &s, const char *message, int width)
{
    int t = time(), sz = strlen(message);
	s << message;
	width--;
	while (sz++ < width)
	    s << " ";
    s << " : " << t << std::endl;
    reset();
}
