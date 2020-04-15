#pragma once
#pragma warning(disable:4996)

#include <cassert>

#include <algorithm>
#include <chrono>
#include <iterator>
#include <string>
#include <thread>


#include "include/distributor.hpp"
#include "include/logger.hpp"
#include "include/thread_synchronizer.hpp"


namespace entity
{

template<typename T>
T rand_between( const T & minimum, const T & maximum );

class philosopher
{
private:
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

        size_t index;
        size_t meals_remaining;
    };


    philosopher( const settings & settings, 
                 control::thread_synchronizer & sync,
                 control::distributor  & dist)
        : m_settings( settings ), 
          m_log( settings.name.c_str() ), 
          m_sync( sync ),
          m_distributor(dist)
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

    const event_log::logger & get_logger() const
    {
        return m_log;
    }

private:
    void wait( time_t time )
    {
        std::this_thread::sleep_for( time );
    }

    control::distributor::forks take_forks()
    {
        return m_distributor.take_forks( m_settings.index );
    }

    void log_activity( activity_type activity )
    {
        m_log.startActivity( activity );

        switch( activity )
        {
        case activity_type::eat:
            wait( rand_between( m_settings.eating.minimum, m_settings.eating.maximum ) );
            break;
        case activity_type::eatFailure:
        case activity_type::think:
        default:
            wait( rand_between( m_settings.eating.minimum, m_settings.eating.maximum ) );
            break;
        }

        m_log.endActivity( activity );
    }

    void think( activity_type activity )
    {
        log_activity( activity );
    }

    activity_type eat(const control::distributor::forks & forks,  size_t & meals_remaining )
    {
        if( forks.is_taken( ) )
        {
            log_activity( activity_type::eat );
            --meals_remaining;
            return activity_type::think;
        }
        
        return activity_type::eatFailure;
    }

    void start_dinner()
    {
        m_sync.wait();

        size_t meals_remaining = m_settings.meals_remaining;

        activity_type activity = activity_type::think;

        while( meals_remaining != 0u )
        {
            think( activity );
            auto forks = take_forks( );
            activity = eat(forks, meals_remaining);
        }
    }

    std::thread m_thread;

    settings m_settings;

    event_log::logger m_log;

    control::thread_synchronizer & m_sync;

    control::distributor & m_distributor;
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
        philosophers.emplace_back( *it, sync, distributor);
    }

    return philosophers;
}

inline philosopher::time_range make_time_range( philosopher::time_t min, philosopher::time_t max )
{
    return { min, max };
}

inline philosopher::settings make_philosophers_settings( const std::string & name, const philosopher::time_range & thinking_range,
                                            const philosopher::time_range & eating_range, size_t index, size_t meals_remaining )
{
    return { name, thinking_range, eating_range, index, meals_remaining };
}

template<typename T>
T rand_between( const T & minimum, const T & maximum )
{
    T time = minimum + (T)rand( );
    time %= (maximum - minimum);
    return std::max( time, minimum );
}

} // namespace entity