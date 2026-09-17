#include "BucketSort.h"
#include <algorithm>

SortResult bucketSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    const std::size_t n = sortedValues.size();
    if (n == 0) return result;

    int minVal = sortedValues[0];
    int maxVal = sortedValues[0];

    for (std::size_t i = 1; i < n; ++i) {
        result.events.push_back({
            EventType::Compare,
            i,
            0,
            0
        });
        result.stats.comparisons++;
        if (sortedValues[i] < minVal) minVal = sortedValues[i];
        if (sortedValues[i] > maxVal) maxVal = sortedValues[i];
    }

    constexpr int K = 10;
    std::vector<std::vector<int>> buckets(K);

    int range = maxVal - minVal;
    if (range == 0) range = 1;

    for (std::size_t i = 0; i < n; ++i) {
        double normalized = static_cast<double>(sortedValues[i] - minVal) / range;
        int bucketIdx = static_cast<int>(normalized * (K - 1));
        if (bucketIdx < 0) bucketIdx = 0;
        if (bucketIdx >= K) bucketIdx = K - 1;

        buckets[static_cast<std::size_t>(bucketIdx)].push_back(sortedValues[i]);
        result.events.push_back({
            EventType::Compare,
            i,
            i,
            0
        });
        result.stats.comparisons++;
    }

    std::size_t idx = 0;
    for (std::size_t b = 0; b < static_cast<std::size_t>(K); ++b) {
        for (int val : buckets[b]) {
            sortedValues[idx] = val;
            result.events.push_back({
                EventType::Overwrite,
                idx,
                0,
                val
            });
            result.stats.swaps++;
            idx++;
        }
    }

    // Sort the concatenated array with Insertion Sort (which is very fast on mostly sorted arrays)
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
