#pragma once

#include <atomic>
#include <functional>
#include <vector>

#include "include/details/unique_take.hpp"
#include "include/fork.hpp"

namespace control
{

class distributor
{
public:
    class forks
    {
    public:
        forks( ) = default;


        forks( entity::fork & left, entity::fork & right, std::function<void( void )> callback )
            : m_left( ),
              m_right( ),
              m_is_taken( false ),
              m_on_destruct( callback )
        {
            details::unique_take take_left( left );
            details::unique_take take_right( right );

            if( take_left && take_right )
            {
                m_left     = std::move( take_left );
                m_right    = std::move( take_right );
                m_is_taken = true;

            }
        }

        ~forks( )
        {
            if( m_on_destruct && m_is_taken )
            {
                m_on_destruct( );
            }
        }

        bool is_taken( ) const
        {
            return m_is_taken;
        }

    private:
        details::unique_take m_left;
        details::unique_take m_right;

        bool m_is_taken = false;;

        std::function<void( void )> m_on_destruct;
    };

public:
    distributor( std::vector<entity::fork> & forks )
        : m_forks( forks ),
          m_eat_queue( m_forks.size( ) )
    { }

    forks take_forks( size_t phil_index )
    {
        bool can_take = check_left_phil( phil_index );
        if ( can_take )
        {
            return { left( phil_index ), right( phil_index ), [this, phil_index]( ) { ++m_eat_queue[phil_index]; } };
        }
        
        return { };
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

    entity::fork & left( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? next_index_of( phil_index ) : phil_index];
    }

    entity::fork & right( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? phil_index : next_index_of( phil_index )];
    }

    bool check_left_phil( size_t phil_ind )
    {
        return (m_eat_queue[prev_index_of( phil_ind )].load( std::memory_order_acquire ) >=
                m_eat_queue[phil_ind].load( std::memory_order_acquire ));
    }

    std::vector<entity::fork> & m_forks;

    std::vector<std::atomic_size_t> m_eat_queue;
};

} // namespace control