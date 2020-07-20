#pragma once
#pragma warning(disable:4996)

#include <cassert>

#include <algorithm>
#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <thread>


#include "distributors.hpp"
#include "logger.hpp"
#include "thread_synchronizer.hpp"


namespace entity
{

template<typename T = size_t>
T rand_between( std::uniform_int_distribution<T> & distr );

class philosopher
{
private:
    using activity_type = event_log::activity_type;
    using time_t = std::chrono::milliseconds;

public:

    struct settings
    {
        std::string name;
        //time_range full_thinking;
        std::uniform_int_distribution<> thinking_distr;
        std::uniform_int_distribution<> eating_distr;

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

    std::unique_ptr<control::distributor::forks> take_forks()
    {
        return m_distributor.take_forks( m_settings.index );
    }

    void log_activity( activity_type activity )
    {
        m_log.startActivity( activity );

        switch( activity )
        {
        case activity_type::eat:
            wait( std::chrono::milliseconds( rand_between( m_settings.eating_distr ) ) );
            break;
        case activity_type::eatFailure:
        case activity_type::think:
        default:
            wait( std::chrono::milliseconds( rand_between( m_settings.thinking_distr ) ) );
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
            activity = eat(*forks, meals_remaining);
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


inline philosopher::settings make_philosophers_settings( const std::string & name, const std::uniform_int_distribution<> &  thinking_distr,
                                            const std::uniform_int_distribution<> & eating_distr, size_t index, size_t meals_remaining )
{
    return { name, std::move(thinking_distr), std::move(eating_distr), index, meals_remaining };
}

template<typename T>
T rand_between( std::uniform_int_distribution<T> & distr )
{
    static std::random_device rd; 
    static std::mt19937 eng( rd( ) );

    return distr( eng );
}

} // namespace entity
