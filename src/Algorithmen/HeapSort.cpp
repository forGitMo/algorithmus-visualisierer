#include "HeapSort.h"
#include <algorithm>

void heapify(std::vector<int>& values, int n, int i, SortResult& result) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n) {
        result.events.push_back({
            EventType::Compare,
            static_cast<std::size_t>(left),
            static_cast<std::size_t>(largest),
            0
        });
        result.stats.comparisons++;

        if (values[left] > values[largest]) {
            largest = left;
        }
    }

    if (right < n) {
        result.events.push_back({
            EventType::Compare,
            static_cast<std::size_t>(right),
            static_cast<std::size_t>(largest),
            0
        });
        result.stats.comparisons++;

        if (values[right] > values[largest]) {
            largest = right;
        }
    }

    if (largest != i) {
        std::swap(values[i], values[largest]);
        result.events.push_back({
            EventType::Swap,
            static_cast<std::size_t>(i),
            static_cast<std::size_t>(largest),
            0
        });
        result.stats.swaps++;

        heapify(values, n, largest, result);
    }
}

SortResult heapSort(const std::vector<int>& values) {
    SortResult result;
    std::vector<int> sortedValues = values;
    int n = static_cast<int>(sortedValues.size());

    // Max Heap erstellen
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(sortedValues, n, i, result);
    }

    // Elemente nacheinander aus dem Heap entnehmen
    for (int i = n - 1; i > 0; i--) {
        std::swap(sortedValues[0], sortedValues[i]);
        result.events.push_back({
            EventType::Swap,
            0,
            static_cast<std::size_t>(i),
            0
        });
        result.stats.swaps++;

        // Element an Index i ist nun an seiner endgültigen, sortierten Position
        result.events.push_back({
            EventType::MarkSorted,
            static_cast<std::size_t>(i),
            0,
            0
        });

        heapify(sortedValues, i, 0, result);
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