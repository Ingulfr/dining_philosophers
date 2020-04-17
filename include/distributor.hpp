#pragma once

#include <atomic>
#include <functional>
#include <type_traits>
#include <vector>

#include "include/details/unique_take.hpp"
#include "include/fork.hpp"

namespace control::details
{

template<bool Condition, typename Type>
struct conditional_member
{
    conditional_member( ) = default;

    conditional_member(Type && other ) 
        : value(std::move(other ) )
    { }

    Type value;
};

} // namespace control::details

namespace control
{

struct simple_logic { };

struct queue_hungers { };

//, typename = std::enable_if_t<std::is_same_v<DistributingStrategy, queue_hungers>>
template<typename DistributingStrategy>
class distributor
{
private:
    using is_queue_hungers = std::is_same<DistributingStrategy, queue_hungers>;

    using queue = std::vector<std::atomic_size_t>;

public:
    class forks
    {
    public:
        template<typename = std::enable_if_t<is_queue_hungers::value>>
        forks( )
            : m_left( ),
              m_right( ),
              m_is_taken( false ),
              m_on_destruct()
        { }

        template<typename = std::enable_if_t<is_queue_hungers::value>>
        forks( entity::fork & left, entity::fork & right, std::function<void( void )> callback )
            : m_left( ),
              m_right( ),
              m_is_taken( false ),
              m_on_destruct( std::move(callback) )
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

        template<typename = std::enable_if_t<!is_queue_hungers::value>>
        forks( entity::fork & left, entity::fork & right)
            : m_left( ),
            m_right( ),
            m_is_taken( false )
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
            if constexpr( is_queue_hungers::value )
            {
                if ( m_on_destruct.value && m_is_taken )
                {
                    m_on_destruct.value( );
                }
            }
        }

        bool is_taken( ) const
        {
            return m_is_taken;
        }

    private:
        details::unique_take m_left;
        details::unique_take m_right;

        bool m_is_taken = false;

        control::details::conditional_member<is_queue_hungers::value, std::function<void( void )>> m_on_destruct;
    };

public:
    distributor( std::vector<entity::fork> & forks )
        : m_forks( forks ),
          m_eat_queue( queue(m_forks.size( ) ) )
    { }


    //std::enable_if<is_queue_hungers::value, forks> take_forks( size_t phil_index)
    forks take_forks( size_t phil_index )
    {
        if constexpr ( is_queue_hungers::value )
        {
            bool can_take = check_left_phil( phil_index );

            if ( can_take )
            {
                return { left( phil_index ), right( phil_index ), [this, phil_index]( ) { ++m_eat_queue.value[phil_index]; } };
            }

            return { };
        }
        else
        {
            return { left( phil_index ), right( phil_index ) };
        }
    }

    /*std::enable_if<!is_queue_hungers::value, forks> take_forks( size_t phil_index)
    {
        return { left( phil_index ), right( phil_index ) };
    }*/


private:
    size_t next_index_of( size_t i ) const
    {
        return (i + 1) % m_forks.size( );
    }

    template<typename = std::enable_if_t<is_queue_hungers::value>>
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

    template<typename = std::enable_if_t<is_queue_hungers::value>>
    bool check_left_phil( size_t phil_ind )
    {
        return (m_eat_queue.value[prev_index_of( phil_ind )].load( std::memory_order_acquire ) >=
                m_eat_queue.value[phil_ind].load( std::memory_order_acquire ));
    }

    std::vector<entity::fork> & m_forks;

    control::details::conditional_member<is_queue_hungers::value, queue>  m_eat_queue;
};

} // namespace control