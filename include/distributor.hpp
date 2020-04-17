#pragma once

#include <atomic>
#include <type_traits>
#include <vector>

#include "include/details/forks.hpp"
#include "include/details/unique_take.hpp"

#include "include/fork.hpp"


namespace control
{

template<typename DistributingStrategy>
class distributor
{
public:
    distributor( std::vector<entity::fork> & forks )
        : m_forks( forks ),
          m_eat_queue( queue(m_forks.size( ) ) )
    { }


    control::details::forks<DistributingStrategy> take_forks( size_t phil_index )
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


private:
    using is_queue_hungers = typename control::details::forks<DistributingStrategy>::is_queue_hungers;

    using queue            = std::vector<std::atomic_size_t>;
    using eat_queue        = control::details::conditional_member<is_queue_hungers::value, queue>;


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

    eat_queue m_eat_queue;
};

} // namespace control