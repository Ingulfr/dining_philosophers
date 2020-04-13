#pragma once

#include <chrono>
#include <string>
#include <condition_variable>
#include <mutex>
#include <atomic>

namespace dining_philosophers::utils
{

#include "include/utils/Utils.hpp"

using time_t = std::chrono::milliseconds;

struct time_range
{
    time_t minimum;
    time_t maximum;
};

inline time_range make_time_range( time_t min, time_t max )
{
    return { min, max };
}



struct philosophers_settings
{
    std::string name;
    //time_range full_thinking;
    time_range hungry_thinking;
    time_range eating;

    size_t meals_remaining;
};

inline philosophers_settings make_philosophers_settings(const std::string & name, const time_range & thinking_range, 
                                                        const time_range & eating_range, size_t meals_remaining )
{
    return { name, thinking_range, eating_range, meals_remaining };
}

inline time_t rand_between(const time_range& range )
{
    time_t time = range.minimum + (time_t)rand();
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

    void notify_all( const int count_philosophers )
    {
        while ( m_wait_philosopers.load( std::memory_order_acquire ) != count_philosophers );

        cond_var.notify_all();
    }

private:

    std::condition_variable cond_var;
    std::mutex m;
    std::atomic_int m_wait_philosopers = 0;
};

} // namespace dining_philosophers::utils
