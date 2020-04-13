#pragma once

#include <vector>
#include <memory>

#include "phil_utils.hpp"

#include "fork.hpp"
#include "philosopher.hpp"

namespace dining_philosophers
{

class philosopher_table
{
    using settings = dining_philosophers::utils::philosophers_settings;
    using synchronizer = dining_philosophers::utils::synchronizer;

    using philosopher_t = dining_philosophers::philosophers::philosopher_t;

    using fork_t = dining_philosophers::forks::fork_t;

public:
    philosopher_table( std::vector<settings>::iterator first, std::vector<settings>::iterator last, int meals_remaining )
    {
        size_t count = last - first;
        m_philosophers.reserve( count );
        m_forks.reserve( count );

        for ( size_t i = 0; i < count; ++i )
        {
            m_forks.emplace_back( std::make_shared<fork_t>() );
        }

        for ( auto it = first; it != last; ++it )
        {
            auto ind = it - first;
            std::shared_ptr<fork_t> left( m_forks[ind] );
            std::shared_ptr<fork_t> right( m_forks[(ind + 1) % count] );

            if ( ind % 2 )
                swap( left, right );

            m_philosophers.emplace_back( *it, m_sync, left, right, meals_remaining );
        }
    }

    void start_dinner( const int count_philosophers )
    {
        m_sync.notify_all( count_philosophers );

        for ( auto& phil : m_philosophers )
        {
            phil.join();
        }
    }

    void wait_end_dinner()
    {
        bool is_done = true;
        do
        {
            is_done = true;
            dining_philosophers::utils::wait( 50 );

            for ( auto& phil : m_philosophers )
            {
                is_done &= phil.done();
            }

        } while ( !is_done );
    }

    void print_dinner_logs()
    {
        for ( auto& phil : m_philosophers )
        {
            phil.logger().printSummary();
        }
    }

private:
    std::vector<philosopher_t> m_philosophers;

    std::vector<std::shared_ptr<fork_t>> m_forks;

    synchronizer m_sync;
};

} // namespace dining_philosophers