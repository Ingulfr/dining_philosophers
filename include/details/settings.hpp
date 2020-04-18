#pragma once

#include <random>
#include <string>

namespace entity::details
{

struct settings
{
    std::string name;

    std::uniform_int_distribution<> thinking_distr;
    std::uniform_int_distribution<> eating_distr;

    size_t index;
    size_t meals_remaining;
};


inline settings make_settings( const std::string & name,
                                                                               const std::uniform_int_distribution<> & thinking_distr,
                                                                               const std::uniform_int_distribution<> & eating_distr,
                                                                               size_t index,
                                                                               size_t meals_remaining )
{
    return { name, std::move( thinking_distr ), std::move( eating_distr ), index, meals_remaining };
}


template<typename T>
T rand_between( std::uniform_int_distribution<T> & distr )
{
    static std::random_device rd;
    static std::mt19937 eng( rd( ) );

    return distr( eng );
}

} // namespace control::details

