#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <vector>

#include "include/details/unique_take.hpp"
#include "include/fork.hpp"

namespace control
{

class base_distributor
{
public:
    class forks
    {
    public:
        forks( ) = default;

        forks( forks && other ) = default;

        forks( entity::fork & left, entity::fork & right)
            : m_left( ),
              m_right( ),
              m_is_taken( false )
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

        virtual ~forks( )
        { }


        bool is_taken( ) const
        {
            return m_is_taken;
        }


    protected:
        details::unique_take m_left;
        details::unique_take m_right;

        bool m_is_taken = false;;
    };


public:
    base_distributor( std::vector<entity::fork> & forks )
        : m_forks( forks )
    { }

    
    virtual std::unique_ptr<forks> take_forks( size_t phil_index )
    {
        return std::make_unique<forks>(forks( left( phil_index ), right( phil_index )));
    }


protected:
    size_t next_index_of( size_t i ) const
    {
        return (i + 1) % m_forks.size( );
    }


    entity::fork & left( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? next_index_of( phil_index ) : phil_index];
    }

    entity::fork & right( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? phil_index : next_index_of( phil_index )];
    }


    std::vector<entity::fork> & m_forks;
};



class queue_distributor : public base_distributor
{
public:
    class forks : public base_distributor::forks
    {
    public:
        forks( ) = default;

        forks( forks && other ) = default;

        forks( entity::fork & left, entity::fork & right, std::function<void( void )> callback )
            : base_distributor::forks(left, right ),
              m_on_destruct( callback )
        {
            details::unique_take take_left( left );
            details::unique_take take_right( right );

            if ( take_left && take_right )
            {
                m_left = std::move( take_left );
                m_right = std::move( take_right );
                m_is_taken = true;

            }
        }

        ~forks( )
        {
            if ( m_on_destruct && m_is_taken )
            {
                m_on_destruct( );
            }
        }

    protected:
        std::function<void( void )> m_on_destruct;
    };

public:
    queue_distributor( std::vector<entity::fork> & forks )
        : base_distributor( forks ),
        m_eat_queue( m_forks.size( ) )
    { }


    std::unique_ptr<typename base_distributor::forks> take_forks( size_t phil_index ) override
    {
        bool can_take = check_left_phil( phil_index ) && check_right_phil( phil_index );
        if ( can_take )
        {
            return std::make_unique<forks>( forks( left( phil_index ), right( phil_index ), [this, phil_index]( ) { ++m_eat_queue[phil_index]; } ) );
        }

        return std::make_unique<forks>( forks() );
    }

protected:
    size_t prev_index_of( size_t i ) const
    {
        return (i - 1 + m_forks.size( )) % m_forks.size( );
    }


    bool check_left_phil( size_t phil_ind )
    {
        return (m_eat_queue[prev_index_of( phil_ind )].load( std::memory_order_acquire ) >=
                 m_eat_queue[phil_ind].load( std::memory_order_acquire ));
    }

    bool check_right_phil( size_t phil_ind )
    {
        return (m_eat_queue[next_index_of( phil_ind )].load( std::memory_order_acquire ) >=
                 m_eat_queue[phil_ind].load( std::memory_order_acquire ));
    }


    std::vector<std::atomic_size_t> m_eat_queue;
};


using distributor  = base_distributor;

using qdistributor = queue_distributor;

} // namespace control
