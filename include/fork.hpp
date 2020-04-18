#pragma once

#include <atomic>
#include <mutex>

namespace entity
{

class fork
{
public:
    bool take()
    {
        bool cur_flag = false;
        if ( m_is_taken.load( std::memory_order::memory_order_acquire ) == false )
        {
            m_is_taken.store( true, std::memory_order::memory_order_release );
            cur_flag = true;
        }

        return  cur_flag;
    }

    void give()
    {
        if ( m_is_taken.load( std::memory_order::memory_order_acquire ) == true )
        {
            m_is_taken.store( false, std::memory_order::memory_order_release );
        }
    }

private:
    std::atomic_bool m_is_taken;
};

} // namespace entity