#pragma once

#include <condition_variable>
#include <mutex>
#include <atomic>

namespace control
{

class thread_synchronizer
{
public:
    void wait( )
    {
        std::unique_lock lock( m );
        ++m_wait_philosopers;
        cond_var.wait( lock );
    }

    void notify_all( const int count_philosophers )
    {
        while ( m_wait_philosopers.load( std::memory_order_acquire ) != count_philosophers );

        cond_var.notify_all( );
    }

private:

    std::condition_variable cond_var;
    std::mutex m;
    std::atomic_int m_wait_philosopers = 0;
};

} // namespace control