#pragma once

// we need to put 3rdpart/logger to namespace
// so use this hack :

namespace event_log
{

#include "3rdpart/Utils.hpp"

using logger        = PhilosopherEventLog;
using activity_type = ActivityType;

} // namespace event_log
