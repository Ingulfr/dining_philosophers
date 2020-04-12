#pragma once
#pragma warning(disable:4996)

#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>

#include "Utils.hpp"

#include "fork.hpp"
#include "phil_utils.hpp"

class philosopher_t
{
public:
    philosopher_t( const philosophers_settings& settings, synchronizer & sync, 
        std::shared_ptr<fork_t> left, std::shared_ptr<fork_t> right, int meals_remaining )
        : m_settings( settings ), m_log( settings.name.c_str() ), m_sync(sync), 
          m_left_fork(left), m_right_fork(right), m_meals_remaining( meals_remaining )
    {
        m_thread = std::thread( &philosopher_t::start_dinner, this );
    }

    void join()
    {
        m_thread.join();
    }

    const PhilosopherEventLog& logger()
    {
        return m_log;
    }

    bool done()
    {
        return m_is_done;
    }    

private:
    void wait( Time_t time )
    {
        std::this_thread::sleep_for( time );
    }

    bool take_forks()
    {
        if( !m_left_fork->take() ) 
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
            m_log.startActivity( ActivityType::think );

            this->wait( rand_between( m_settings.hungry_thinking ) );

            m_log.endActivity( ActivityType::think );

            m_is_hungry = true;

            return;
        }

        m_log.startActivity( ActivityType::eatFailure );

        this->wait( rand_between( m_settings.hungry_thinking ) );

        m_log.endActivity( ActivityType::eatFailure );
    }

    void eat()
    {
        m_log.startActivity( ActivityType::eat );

        wait( rand_between( m_settings.eating ) );

        m_log.endActivity( ActivityType::eat );

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

    philosophers_settings m_settings;

    PhilosopherEventLog m_log;

    synchronizer & m_sync;

    std::shared_ptr<fork_t> m_left_fork;

    std::shared_ptr<fork_t> m_right_fork;

    int m_meals_remaining;

    bool m_is_done = false;

    bool m_is_hungry = false;
};

//inline bool philosopher::can_start = false;

//inline std::condition_variable philosopher::start_cond;
