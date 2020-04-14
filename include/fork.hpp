#pragma once

#include <mutex>

namespace entity
{

class fork
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

} // namespace entity