#pragma once
#include <vector>
#include <string>
#include "../models/SearchResult.h"

class ISearchAlgorithm {
public:
    virtual ~ISearchAlgorithm() = default;
    virtual std::string getName() const = 0;
    virtual std::string getComplexity() const = 0;
    virtual SearchResult search(const std::vector<int>& values, int target) = 0;
};
