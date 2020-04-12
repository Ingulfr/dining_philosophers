﻿
#include <iostream>

#include <chrono>
#include <vector>

#include "philosopher_table.hpp"
#include "phil_utils.hpp"


using namespace std::chrono_literals;


std::vector<philosophers_settings> phil_settings{ { "Socrates", 10ms, 50ms, 30ms, 70ms, },
                                                  { "Plato", 20ms, 70ms, 25ms, 80ms },
                                                  { "Aristotle", 15ms, 40ms, 10ms, 40ms },
                                                  { "Schopenhauer", 50ms, 100ms, 20ms, 90ms },
                                                  { "Nietzsche", 30ms, 60ms, 20ms, 60ms },
                                                  { "Wittgenstein", 40ms, 90ms, 10ms, 90ms } };

void organize_dinner(const int count_philosophers = 3, const int meals_remaining = 3)
{
    philosopher_table table( phil_settings.begin(), phil_settings.begin() + count_philosophers, meals_remaining );

    table.start_dinner( count_philosophers );

    table.wait_end_dinner();

    table.print_dinner_logs();
}

int main()
{
    organize_dinner(5);
}
