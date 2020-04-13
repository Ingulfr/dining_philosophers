
#include <iostream>

#include <chrono>
#include <vector>

#include "include/philosopher_table.hpp"
#include "include/utils/phil_utils.hpp"


using namespace std::chrono_literals;
using namespace dining_philosophers;

using philosophers_settings = utils::philosophers_settings;

using utils::make_philosophers_settings;
using utils::make_time_range;


std::vector<philosophers_settings> generate_philosophers_settings(size_t meals_remaining )
{
    static std::vector<std::string> names{ "Socrates" ,    "Plato",     "Aristotle",
                                           "Schopenhauer", "Aristotle", "Wittgenstein" };

    static std::vector< utils::time_t> thinking_minimum_times{ 10ms, 20ms, 15ms, 50ms,  30ms, 40ms };
    static std::vector< utils::time_t> thinking_maximum_times{ 50ms, 70ms, 40ms, 100ms, 60ms, 90ms };

    static std::vector< utils::time_t> eating_minimum_times  { 30ms, 25ms, 10ms, 20ms,  20ms, 10ms };
    static std::vector< utils::time_t> eating_maximum_times  { 70ms, 80ms, 40ms, 90ms,  60ms, 90ms };

    std::vector<philosophers_settings> settings;

    settings.reserve( names.size() );

    for ( auto i = 0u; i < names.size(); ++i )
    {
        settings.emplace_back( make_philosophers_settings( names[i], make_time_range( thinking_minimum_times[i], thinking_maximum_times[i]),
                               make_time_range( eating_minimum_times[i], eating_maximum_times[i]), meals_remaining ) );
    }

    return settings;
}

void organize_dinner(const int count_philosophers = 3, const size_t meals_remaining = 3)
{

    std::vector<philosophers_settings> phil_settings( generate_philosophers_settings( meals_remaining ) );

    philosopher_table table( phil_settings.begin(), phil_settings.begin() + count_philosophers);

    table.start_dinner();

    table.wait_end_dinner();

    table.print_dinner_logs();
}

int main()
{
    organize_dinner(5);
}
