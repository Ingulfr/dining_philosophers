#pragma once

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
        return std::make_unique<forks>( forks( left( phil_index ), right( phil_index ) ) );
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

} // namespace control