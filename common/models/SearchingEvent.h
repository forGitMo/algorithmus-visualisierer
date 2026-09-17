#pragma once
#include <cstddef>

enum class SearchEventType {
    Compare,
    MarkLeft,
    MarkRight,
    MarkMiddle,
    Found,
    NotFound
};

struct SearchEvent {
    SearchEventType type{};
    std::size_t index{};
    int value{};
};
