#include "BubbleSort.h"

SortResult bubbleSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    const std::size_t n = sortedValues.size();

    for (std::size_t i = 0; i + 1 < n; ++i) {
        for (std::size_t j = 0; j + 1 < n - i; ++j) {
            result.events.push_back({
                EventType::Compare,
                j,
                j + 1,
                0
            });

            result.stats.comparisons++;

            if (sortedValues[j] > sortedValues[j + 1]) {
                std::swap(sortedValues[j], sortedValues[j + 1]);

                result.events.push_back({
                    EventType::Swap,
                    j,
                    j + 1,
                    0
                });

                result.stats.swaps++;
            }
        }

        result.events.push_back({
            EventType::MarkSorted,
            n - 1 - i,
            0,
            0
        });
    }

    if (n > 0) {
        result.events.push_back({
            EventType::MarkSorted,
            0,
            0,
            0
        });
    }

    return result;
}