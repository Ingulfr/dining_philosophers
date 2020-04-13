#pragma once
#pragma warning(disable:4996)

#include <thread>
#include <mutex>
#include <iterator>
#include <cassert>

#include "include/fork.hpp"
#include "include/utils/phil_utils.hpp"

namespace dining_philosophers::philosophers
{

class philosopher_t
{
using settings      = dining_philosophers::utils::philosophers_settings;
using synchronizer  = dining_philosophers::utils::synchronizer;
using time_type     = dining_philosophers::utils::time_t;

using event_log     = dining_philosophers::utils::PhilosopherEventLog;
using activity_type = dining_philosophers::utils::ActivityType;

using fork_t        = dining_philosophers::forks::fork_t;


public:
    philosopher_t( const settings & settings, 
                   synchronizer & sync,
                   fork_t & left, 
                   fork_t & right)
        : m_settings( settings ), 
          m_log( settings.name.c_str() ), 
          m_sync( sync ),
          m_left_fork( left ), 
          m_right_fork( right )
    {
        m_thread = std::thread( &philosopher_t::start_dinner, this );
    }

    philosopher_t( philosopher_t && other ) = default;
     

    ~philosopher_t()
    {
        assert( ("Thread were not joined!", !m_thread.joinable()) );
    }

    void join()
    {
        m_thread.join();
    }

    const event_log & logger()
    {
        return m_log;
    }

private:
    void wait( time_type time )
    {
        std::this_thread::sleep_for( time );
    }

    bool take_forks()
    {
        if ( !m_left_fork.take() )
            return false;

        if ( !m_right_fork.take() )
        {
            m_left_fork.give();
            return false;
        }

        return true;
    }

    void give_forks()
    {
        m_left_fork.give();

        m_right_fork.give();
    }

    void think()
    {
        if ( !m_is_hungry )
        {
            m_log.startActivity( activity_type::think );

            this->wait( rand_between( m_settings.hungry_thinking ) );

            m_log.endActivity( activity_type::think );

            m_is_hungry = true;

            return;
        }

        m_log.startActivity( activity_type::eatFailure );

        this->wait( rand_between( m_settings.hungry_thinking ) );

        m_log.endActivity( activity_type::eatFailure );
    }

    void eat()
    {
        m_log.startActivity( activity_type::eat );

        wait( rand_between( m_settings.eating ) );

        m_log.endActivity( activity_type::eat );

        --m_settings.meals_remaining;
        m_is_hungry = false;

        give_forks();
    }

    void start_dinner()
    {
        m_sync.wait();

        while ( m_settings.meals_remaining != 0u )
        {
            think();
            if ( take_forks() )
                eat();
        }
    }

    std::thread m_thread;

    settings m_settings;

    event_log m_log;

    synchronizer & m_sync;

    fork_t & m_left_fork;

    fork_t & m_right_fork;

    bool m_is_hungry = false;
};


template<typename Iterator, 
         typename Distributer = dining_philosophers::forks::forks_distributor,
         typename Synchronizer = dining_philosophers::utils::synchronizer>
std::vector<philosopher_t> make_philosophers( Iterator first, Iterator last, Distributer & distributor, Synchronizer & sync )
{
    size_t count = std::distance(first, last);
    std::vector< philosopher_t> philosophers;
    
    philosophers.reserve( count );

    for ( auto it = first; it != last; ++it )
    {
        philosophers.emplace_back( *it, sync, distributor.left(), distributor.right());
    }

    return philosophers;
}

} // namespace dining_philosophers::philosophers