#pragma once

#include <functional>

#include "details/conditional_member.hpp"

#include "fork.hpp"
#include "strategies.hpp"
#include "unique_take.hpp"

namespace control::details
{

template<typename DistributingStrategy>
class forks
{
public:
    using is_queue_hungers = std::is_same<DistributingStrategy, control::strategies::queue_hungers>;

    template<typename = std::enable_if_t<is_queue_hungers::value>>
    forks( )
        : m_left( ),
        m_right( ),
        m_is_taken( false ),
        m_on_destruct( )
    { }

    template<typename = std::enable_if_t<is_queue_hungers::value>>
    forks( entity::fork & left, entity::fork & right, std::function<void( void )> callback )
        : m_left( ),
        m_right( ),
        m_is_taken( false ),
        m_on_destruct( std::move( callback ) )
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

    template<typename = std::enable_if_t<!is_queue_hungers::value>>
    forks( entity::fork & left, entity::fork & right )
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
        if constexpr ( is_queue_hungers::value )
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
    using destruct = control::details::conditional_member<is_queue_hungers::value, std::function<void( void )>>;

    details::unique_take m_left;
    details::unique_take m_right;

    bool m_is_taken = false;

    destruct m_on_destruct;
};

} // namespace control::details
