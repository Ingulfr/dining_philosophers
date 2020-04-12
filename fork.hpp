#pragma once

#include <mutex>

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
