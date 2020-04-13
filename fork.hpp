#pragma once

#include <mutex>

namespace dining_philosophers::forks
{

    class fork_t
    {
    public:
        bool take()
        {
            return locker.try_lock();
        }

        void give()
        {
            locker.unlock();
        }

    private:
        std::mutex locker;
    };

} // dining_philosolhers::forks