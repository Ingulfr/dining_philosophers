#pragma once
#pragma warning(disable:4996)

#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "include/fork.hpp"
#include "include/utils/phil_utils.hpp"

namespace dining_philosophers::philosophers
{


class philosopher_t
{
using settings      = dining_philosophers::utils::philosophers_settings;
using synchronizer  = dining_philosophers::utils::synchronizer;
using time_type     = dining_philosophers::utils::Time_t;

using event_log     = dining_philosophers::utils::PhilosopherEventLog;
using activity_type = dining_philosophers::utils::ActivityType;

using fork_t        = dining_philosophers::forks::fork_t;


public:
    philosopher_t( const settings & settings, synchronizer & sync,
        std::shared_ptr<fork_t> left, std::shared_ptr<fork_t> right, int meals_remaining )
        : m_settings( settings ), m_log( settings.name.c_str() ), m_sync( sync ),
        m_left_fork( left ), m_right_fork( right ), m_meals_remaining( meals_remaining )
    {
        m_thread = std::thread( &philosopher_t::start_dinner, this );
    }

    void join()
    {
        m_thread.join();
    }

    const event_log & logger()
    {
        return m_log;
    }

    bool done()
    {
        return m_is_done;
    }

private:
    void wait( time_type time )
    {
        std::this_thread::sleep_for( time );
    }

    bool take_forks()
    {
        if ( !m_left_fork->take() )
            return false;

        if ( !m_right_fork->take() )
        {
            m_left_fork->give();
            return false;
        }

        return true;
    }

    void give_forks()
    {
        m_left_fork->give();

        m_right_fork->give();
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

        --m_meals_remaining;
        m_is_hungry = false;

        give_forks();
    }

    void start_dinner()
    {
        m_sync.wait();

        while ( m_meals_remaining )
        {
            think();
            if ( take_forks() )
                eat();
        }

        m_is_done = true;
    }

    std::thread m_thread;

    settings m_settings;

    event_log m_log;

    synchronizer& m_sync;

    std::shared_ptr<fork_t> m_left_fork;

    std::shared_ptr<fork_t> m_right_fork;

    int m_meals_remaining;

    bool m_is_done = false;

    bool m_is_hungry = false;
};

} // namespace dining_philosophers::philosophers