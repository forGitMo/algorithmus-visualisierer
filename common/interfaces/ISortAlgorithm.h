#pragma once
#include <vector>
#include <string>
#include "../models/SortResult.h"

class ISortAlgorithm {
public:
    virtual ~ISortAlgorithm() = default;
    virtual std::string getName() const = 0;
    virtual std::string getComplexity() const = 0;
    virtual std::string getDescription() const = 0;
    virtual SortResult sort(const std::vector<int>& values) = 0;
};
