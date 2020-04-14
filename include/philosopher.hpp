#pragma once
#pragma warning(disable:4996)

#include <cassert>

#include <chrono>
#include <iterator>
#include <string>
#include <thread>


#include "include/fork.hpp"
#include "include/logger.hpp"
#include "include/thread_synchronizer.hpp"


namespace entity
{

class philosopher
{
private:
    using synchronizer  = control::thread_synchronizer;

    using fork          = entity::fork;

    using logger        = event_log::logger;
    using activity_type = event_log::activity_type;

public:
    using time_t = std::chrono::milliseconds;

    struct time_range
    {
        time_t minimum;
        time_t maximum;
    };

    struct settings
    {
        std::string name;
        //time_range full_thinking;
        time_range hungry_thinking;
        time_range eating;

        size_t meals_remaining;
    };


    philosopher( const settings & settings, 
                   synchronizer & sync,
                   fork & left, 
                   fork & right)
        : m_settings( settings ), 
          m_log( settings.name.c_str() ), 
          m_sync( sync ),
          m_left_fork( left ), 
          m_right_fork( right )
    {
        m_thread = std::thread( &philosopher::start_dinner, this );
    }

    philosopher( philosopher && other ) = default;
     

    ~philosopher()
    {
        assert( ("Thread were not joined!", !m_thread.joinable()) );
    }

    void join()
    {
        m_thread.join();
    }

    const logger & get_logger()
    {
        return m_log;
    }

private:
    time_t rand_between( const time_range & range )
    {
        time_t time = range.minimum + (time_t)rand( );
        time %= (range.maximum - range.minimum);
        return time;
    }

    void wait( time_t time )
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

    logger m_log;

    synchronizer & m_sync;

    fork & m_left_fork;

    fork & m_right_fork;

    bool m_is_hungry = false;
};



template<typename Iterator, 
         typename Distributer  = control::distributor,
         typename Synchronizer = control::thread_synchronizer>
std::vector<philosopher> make_philosophers( Iterator first, Iterator last, Distributer & distributor, Synchronizer & sync )
{
    size_t count = std::distance(first, last);
    std::vector< philosopher> philosophers;
    
    philosophers.reserve( count );

    for ( auto it = first; it != last; ++it )
    {
        philosophers.emplace_back( *it, sync, distributor.left(), distributor.right());
    }

    return philosophers;
}

inline philosopher::time_range make_time_range( philosopher::time_t min, philosopher::time_t max )
{
    return { min, max };
}

inline philosopher::settings make_philosophers_settings( const std::string & name, const philosopher::time_range & thinking_range,
                                            const philosopher::time_range & eating_range, size_t meals_remaining )
{
    return { name, thinking_range, eating_range, meals_remaining };
}

} // namespace entity