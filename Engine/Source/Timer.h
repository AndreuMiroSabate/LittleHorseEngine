#pragma once

#include <chrono>

class Timer
{
public:
    Timer();

    void Start();

    void Stop();

    double GetMilliseconds() const;


private:
    using clock = std::chrono::steady_clock;

    std::chrono::time_point<clock> m_start;
    std::chrono::time_point<clock> m_end;
    bool m_running;

    template<typename TDuration>
    double GetDuration() const
    {
        if (m_running)
        {
            return std::chrono::duration_cast<TDuration>(clock::now() - m_start).count();
        }
        else
        {
            return std::chrono::duration_cast<TDuration>(m_end - m_start).count();
        }
    }
};

