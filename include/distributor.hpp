#pragma once

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
        forks( fork & left, fork & right )
            : m_left( left ), m_right( right )
        {
            if( m_left.take( ) )
            {
                if ( m_right.take( ) )
                    m_is_taken = true;
                else
                    m_left.give( );
            }
        }

        ~forks( )
        {
            if ( m_is_taken )
            {
                m_left.give( );
                m_right.give( );
            }
        }

        bool is_taken( ) const
        {
            return m_is_taken;
        }

    private:
        fork & m_left;
        fork & m_right;

        bool m_is_taken = false;
    };

public:
    distributor( std::vector<fork> & forks )
        : m_forks( forks )
    { }

    forks take_forks( size_t phil_index )
    {
        return { left( phil_index ), right( phil_index) };
    }

private:
    size_t next_index_of( size_t i ) const
    {
        return (i + 1) % m_forks.size( );
    }

    fork & left( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? next_index_of( phil_index ) : phil_index];
    }

    fork & right( size_t phil_index )
    {
        return m_forks[(phil_index % 2) ? phil_index : next_index_of( phil_index )];
    }

    std::vector<fork> & m_forks;
};

} // namespace control