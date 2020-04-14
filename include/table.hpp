#pragma once

#include <vector>
#include <iterator>

#include "include/distributor.hpp"
#include "include/fork.hpp"
#include "include/philosopher.hpp"
#include "include/thread_synchronizer.hpp"


namespace control
{

using entity::make_philosophers;

class table
{
    using settings = entity::philosopher::settings;

    using philosopher_t = entity::philosopher;

    using fork_t = entity::fork;


public:
    table( std::vector<settings>::iterator first, std::vector<settings>::iterator last )
        : m_forks( std::vector<fork_t>( std::distance( first, last ) ) ),
        m_distributor( m_forks ),
        m_sync( ),
        m_philosophers( make_philosophers( first, last, m_distributor, m_sync ) )
    { }


    void start_dinner( )
    {
        m_sync.notify_all( m_philosophers.size( ) );

        for ( auto & phil : m_philosophers )
        {
            phil.join( );
        }
    }


    void print_dinner_logs( )
    {
        for ( auto & phil : m_philosophers )
        {
            phil.get_logger( ).printSummary( );
        }
    }

private:
    std::vector<fork_t> m_forks;

    distributor m_distributor;

    thread_synchronizer m_sync;

    std::vector<philosopher_t> m_philosophers;
};

} // namespace control