#pragma once

#include <vector>
#include <iterator>

#include "include/utils/phil_utils.hpp"

#include "include/fork.hpp"
#include "include/philosopher.hpp"

namespace dining_philosophers
{

class philosopher_table
{
    using settings         = dining_philosophers::utils::philosophers_settings;
    using synchronizer     = dining_philosophers::utils::synchronizer;

    using philosopher_t    = dining_philosophers::philosophers::philosopher_t;

    using fork_t           = dining_philosophers::forks::fork_t;
    using forks_distributor = dining_philosophers::forks::forks_distributor;

public:
    philosopher_table( std::vector<settings>::iterator first, std::vector<settings>::iterator last, int meals_remaining )
        : m_forks(std::vector<fork_t>( std::distance( first, last ) ) ), distributor(m_forks)
    {
        size_t count = last - first;
        m_philosophers.reserve( count );

        for ( auto it = first; it != last; ++it )
        {
            m_philosophers.emplace_back( *it, m_sync, distributor.left(), distributor.right(), meals_remaining );
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

    std::vector<fork_t> m_forks;

    forks_distributor distributor;

    synchronizer m_sync;
};

} // namespace dining_philosophers