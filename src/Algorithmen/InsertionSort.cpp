#include "InsertionSort.h"
#include <utility>

SortResult insertionSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    const std::size_t n = sortedValues.size();

    for (std::size_t i = 1; i < n; ++i) {
        std::size_t j = i;
        while (j > 0) {
            result.events.push_back({
                EventType::Compare,
                j - 1,
                j,
                0
            });
            result.stats.comparisons++;

            if (sortedValues[j] < sortedValues[j - 1]) {
                std::swap(sortedValues[j], sortedValues[j - 1]);
                result.events.push_back({
                    EventType::Swap,
                    j - 1,
                    j,
                    0
                });
                result.stats.swaps++;
                j--;
            } else {
                break;
            }
        }
    }

    // Mark all elements as sorted at the end
    for (std::size_t i = 0; i < n; ++i) {
        result.events.push_back({
            EventType::MarkSorted,
            i,
            0,
            0
        });
    }

    return result;
}
