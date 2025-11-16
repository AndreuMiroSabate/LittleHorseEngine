#include "Globals.h"
#include "Timer.h"

Timer::Timer()
{
	m_running = false;
}

void Timer::Start()
{
	m_start = clock::now();
	m_running = true;
}

void Timer::Stop()
{
	m_end = clock::now();
	m_running = false;
}

double Timer::GetMilliseconds() const
{
	return GetDuration<std::chrono::milliseconds>();
}



