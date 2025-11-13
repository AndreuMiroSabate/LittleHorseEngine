#include "Globals.h"
#include "Timer.h"

std::chrono::time_point<Timer> Timer::now()
{
    LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);


	return time_point(duration);
}

LARGE_INTEGER Timer::count()
{
	return ;
}
