#pragma once

#include <utility>

namespace control::details
{

template<bool Condition, typename Type>
struct conditional_member
{
    conditional_member( ) = default;

    conditional_member( Type && other )
        : value( std::move( other ) )
    { }

    Type value;
};

} // namespace control::details