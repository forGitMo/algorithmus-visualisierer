#pragma once
#include <vector>
#include "SortingEvent.h"
#include "SortingStats.h"

struct SortResult {
    std::vector<SortingEvent> events;
    SortingStats stats;
};
