#pragma once
#include <cstddef>

enum class EventType {
    Compare,
    Swap,
    Overwrite,
    MarkSorted,
    SetPivot,
    ClearHighlight
};

struct SortingEvent {
    EventType   type{};
    std::size_t indexA{};
    std::size_t indexB{};
    int         value{};
};
