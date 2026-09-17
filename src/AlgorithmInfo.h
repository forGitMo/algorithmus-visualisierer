#pragma once

#include <vector>
#include <string>

#include "SortingEvent.h"
#include "SortingStats.h"

struct SortResult {
    std::vector<SortingEvent> events;
    SortingStats stats;
};

struct AlgorithmInfo {
    std::string name;
    std::string description;
    std::string bestCase;
    std::string averageCase;
    std::string worstCase;
    std::string memorie;
};
