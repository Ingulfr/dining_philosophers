#pragma once

#include <chrono>
#include <string>
#include <condition_variable>
#include <mutex>
#include <atomic>

using Time_t = std::chrono::milliseconds;

struct time_range
{
    Time_t minimum;
    Time_t maximum;
};

struct philosophers_settings
{
    std::string name;
    //time_range full_thinking;
    time_range hungry_thinking;
    time_range eating;
};

inline Time_t rand_between( time_range& range )
{
    Time_t time = range.minimum + (Time_t)rand();
    time %= (range.maximum - range.minimum);
    return time;
}

class synchronizer
{
public:
    void wait()
    {
        std::unique_lock lock( m );
        ++m_wait_philosopers;
        cond_var.wait( lock );
    }

    void notify_all(const int count_philosophers )
    {
        while ( m_wait_philosopers.load( std::memory_order_acquire ) != count_philosophers );

        cond_var.notify_all();
    }

private:

    std::condition_variable cond_var;
    std::mutex m;
    std::atomic_int m_wait_philosopers = 0;
};

