#pragma once

#include <vector>
#include <iterator>

#include "include/distributor.hpp"
#include "include/fork.hpp"
#include "include/philosopher.hpp"
#include "include/thread_synchronizer.hpp"


namespace control
{

class table
{
public:
    template<typename Settings = entity::philosopher::settings,
             typename Iterator = typename std::vector<Settings>::iterator>
    table( Iterator first, Iterator last )
        : m_forks( std::vector<entity::fork>( std::distance( first, last ) ) ),
        m_distributor( m_forks ),
        m_sync( ),
        m_philosophers( entity::make_philosophers( first, last, m_distributor, m_sync ) )
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
    std::vector<entity::fork> m_forks;

    control::distributor m_distributor;

    control::thread_synchronizer m_sync;

    std::vector<entity::philosopher> m_philosophers;
};

} // namespace control