#ifndef CTIMER_HPP
#define CTIMER_HPP

#include <chrono>

using namespace std::chrono;

class CTimer
{
public:
    CTimer()
    {
        last = steady_clock::now();
    }
    float Mark()
    {
        const auto old = last;
        last = steady_clock::now();
        const duration<float> frameTime = last - old;
        return frameTime.count();
    }
    float Peek() const
    {
        return duration<float>(steady_clock::now() - last).count();
    }
private:
    std::chrono::steady_clock::time_point last;
};

#endif