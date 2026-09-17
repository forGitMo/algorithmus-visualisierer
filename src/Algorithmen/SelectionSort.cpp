#include "SelectionSort.h"

SortResult selectionSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    const std::size_t n = sortedValues.size();

    for (std::size_t i = 0; i < n; ++i) {
        std::size_t minIndex = i;

        for (std::size_t j = i + 1; j < n; ++j) {
            result.events.push_back({
                EventType::Compare,
                j,
                minIndex,
                0
            });

            result.stats.comparisons++;

            if (sortedValues[j] < sortedValues[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            std::swap(sortedValues[i], sortedValues[minIndex]);

            result.events.push_back({
                EventType::Swap,
                i,
                minIndex,
                0
            });

            result.stats.swaps++;
        }

        result.events.push_back({
            EventType::MarkSorted,
            i,
            0,
            0
        });
    }

    return result;
}
