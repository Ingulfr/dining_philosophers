
#include <iostream>

#include <vector>

#include "include/distributor.hpp"
#include "include/philosopher.hpp"
#include "include/table.hpp"
#include "include/strategies.hpp"

using simple_dist = control::distributor<control::strategies::simple_logic>;
using smart_dist = control::distributor<control::strategies::queue_hungers>;

template<typename Distributor>
using philosophers_settings = typename entity::philosopher<Distributor>::settings;

using entity::make_philosophers_settings;

template<typename Distributor>
std::vector<philosophers_settings<Distributor>> generate_philosophers_settings(size_t meals_remaining )
{

    static std::vector<std::string> names{ "Socrates" ,    "Plato",     "Aristotle",
                                           "Schopenhauer", "Nietzsche", "Wittgenstein" };

    static std::vector< size_t> thinking_minimum_times{ 10, 20, 15, 50,  30, 40 };
    static std::vector< size_t> thinking_maximum_times{ 50, 70, 40, 100, 60, 90 };

    static std::vector< size_t> eating_minimum_times  { 30, 25, 10, 20,  20, 10 };
    static std::vector< size_t> eating_maximum_times  { 70, 80, 40, 90,  60, 90 };

    std::vector<philosophers_settings<Distributor>> settings;

    settings.reserve( names.size() );

    for ( auto i = 0u; i < names.size(); ++i )
    {
        settings.emplace_back( make_philosophers_settings<Distributor>( names[i], 
                                                                        std::uniform_int_distribution<>( thinking_minimum_times[i],
                                                                                                         thinking_maximum_times[i] ),
                                                                        std::uniform_int_distribution<>( eating_minimum_times[i], 
                                                                                                         eating_maximum_times[i] ), 
                                                                        i, 
                                                                        meals_remaining ) );
    }

    return settings;
}

template<typename DistributingStrategy>
void organize_dinner(const int count_philosophers = 3, const size_t meals_remaining = 5)
{

    std::vector<philosophers_settings<control::distributor<DistributingStrategy>>> phil_settings( generate_philosophers_settings<control::distributor<DistributingStrategy>>( meals_remaining ) );

    control::table<DistributingStrategy> table( phil_settings.begin(), phil_settings.begin() + count_philosophers);

    table.start_dinner();

    table.print_dinner_logs();
}

int main()
{
    std::cout << "queue of hungers logic:\n";
    organize_dinner<control::strategies::queue_hungers>(5);
    std::cout << "\n\nsimple logic:\n";
    organize_dinner<control::strategies::simple_logic>( 5 );
}
