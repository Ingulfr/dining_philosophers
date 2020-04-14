#pragma once

#include <vector>

#include "include/fork.hpp"

namespace control
{

class distributor
{
private:
using fork_t = entity::fork;


public:
    distributor( std::vector<fork_t> & forks )
        : m_forks( forks ), m_left_index( 0 )
    { }

    fork_t & left( )
    {
        return m_forks[(m_left_index % 2) ? next_index_of( m_left_index ) : m_left_index];
    }

    fork_t & right( )
    {
        return m_forks[(m_left_index % 2) ? m_left_index : next_index_of( m_left_index )];
    }

    void next( )
    {
        m_left_index = next_index_of( m_left_index );
    }


private:
    size_t next_index_of( size_t i ) const
    {
        return (i + 1) % m_forks.size( );
    }


    std::vector<fork_t> & m_forks;

    size_t m_left_index;
};

} // namespace control