#pragma once

#include <chrono>

class Timer
{
public:

	std::chrono::time_point<Timer> now() noexcept;
	LARGE_INTEGER count();

private:

	double  rep;
	const bool Is_steady = true;
	using time_point = std::chrono::time_point<Timer>;
	std::ratio<1> period;
	std::chrono::duration<float, std::ratio<1>> duration;
};

