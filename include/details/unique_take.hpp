#pragma once

#include "fork.hpp"

namespace control::details
{

class unique_take
{
public:
    unique_take( )
        : m_fork( nullptr ),
        m_is_taken( false )
    { }

    unique_take( entity::fork & fork )
        : m_fork( &fork ),
        m_is_taken( false )
    {
        m_is_taken = m_fork->take( );
    }

    unique_take( unique_take && other )
        : m_fork( other.m_fork ),
        m_is_taken( other.m_is_taken )
    {
        other.m_fork = nullptr;
        other.m_is_taken = false;
    }

    unique_take & operator=( unique_take && other )
    {
        m_fork = other.m_fork;
        other.m_fork = nullptr;

        m_is_taken = other.m_is_taken;
        other.m_is_taken = false;

        return *this;
    }

    ~unique_take( )
    {
        if ( m_is_taken )
        {
            m_fork->give( );
        }
    }

    operator bool( ) const
    {
        return m_is_taken;
    }


private:
    entity::fork * m_fork;

    bool m_is_taken;
};

} // namespace control::detail
