#include "RadixSort.h"
#include <algorithm>

static void countingSort(std::vector<int>& arr, int exp, SortResult& result) {
    const std::size_t n = arr.size();
    std::vector<int> output(n);
    int count[10] = {0};

    for (std::size_t i = 0; i < n; ++i) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
        result.events.push_back({
            EventType::Compare,
            i,
            i,
            0
        });
        result.stats.comparisons++;
    }

    for (int i = 1; i < 10; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = static_cast<int>(n) - 1; i >= 0; --i) {
        int digit = (arr[static_cast<std::size_t>(i)] / exp) % 10;
        output[static_cast<std::size_t>(count[digit] - 1)] = arr[static_cast<std::size_t>(i)];
        count[digit]--;
        result.events.push_back({
            EventType::Compare,
            static_cast<std::size_t>(i),
            static_cast<std::size_t>(i),
            0
        });
        result.stats.comparisons++;
    }

    for (std::size_t i = 0; i < n; ++i) {
        arr[i] = output[i];
        result.events.push_back({
            EventType::Overwrite,
            i,
            0,
            output[i]
        });
        result.stats.swaps++;
    }
}

SortResult radixSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    const std::size_t n = sortedValues.size();
    if (n == 0) return result;

    int maxVal = sortedValues[0];
    for (std::size_t i = 1; i < n; ++i) {
        result.events.push_back({
            EventType::Compare,
            i,
            0,
            0
        });
        result.stats.comparisons++;
        if (sortedValues[i] > maxVal) {
            maxVal = sortedValues[i];
        }
    }

    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        countingSort(sortedValues, exp, result);
    }

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
