#pragma once

#include <atomic>
#include <functional>
#include <vector>

#include "include/fork.hpp"

namespace control
{

class distributor
{
private:
using fork = entity::fork;

public:
    class forks
    {
    public:
        forks( fork & left, fork & right, bool can_take, std::function<void( void )> callback )
            : m_left( left ),
            m_right( right ),
            m_is_taken( can_take ),
            m_queue_increment( callback )
        {
            if ( can_take )
            {
                if ( m_left.take( ) )
                {
                    if ( !m_right.take( ) )
                    {
                        m_left.give( );
                        m_is_taken = false;
                    }
                }
                else
                    m_is_taken = false;
            }
        }

        ~forks( )
        {
            if ( m_is_taken )
            {
                m_left.give( );
                m_right.give( );
                m_queue_increment( );
            }
        }

        bool is_taken( ) const
        {
            return m_is_taken;
        }

    private:
        fork & m_left;
        fork & m_right;

        bool m_is_taken;

        std::function<void( void )> m_queue_increment;
    };

public:
    distributor( std::vector<fork> & forks )
        : m_forks( forks ),
          m_eat_queue( m_forks.size( ) )
    { }

    forks take_forks( size_t phil_index )
    {
        bool can_take = check_left_phil( phil_index );
        auto m_queue_increment = [this, phil_index]( ) { ++m_eat_queue[phil_index]; };
        return { left( phil_index ), right( phil_index), can_take, m_queue_increment };
    }

private:
    size_t next_index_of( size_t i ) const
    {
        return (i + 1) % m_forks.size( );
    }

    size_t prev_index_of( size_t i ) const
    {
        return (i - 1 + m_forks.size( )) % m_forks.size( );
    }

    fork & left( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? next_index_of( phil_index ) : phil_index];
    }

    fork & right( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? phil_index : next_index_of( phil_index )];
    }

    bool check_left_phil( size_t phil_ind )
    {
        return (m_eat_queue[prev_index_of( phil_ind )].load( std::memory_order_acquire ) >=
                m_eat_queue[phil_ind].load( std::memory_order_acquire ));
    }

    std::vector<fork> & m_forks;

    std::vector<std::atomic_size_t> m_eat_queue;
};

} // namespace control