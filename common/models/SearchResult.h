#pragma once
#include <vector>
#include "SearchingEvent.h"
#include "SearchingStats.h"

struct SearchResult {
    bool found{false};
    std::size_t foundIndex{0};
    std::vector<SearchEvent> events;
    SearchingStats stats;
};
