#pragma once

#include <vector>
#include <random>

class NonCopyableNonMovableMarker
{
protected:
    NonCopyableNonMovableMarker() = default;
    ~NonCopyableNonMovableMarker() = default;

    NonCopyableNonMovableMarker(const NonCopyableNonMovableMarker&) = delete;
    NonCopyableNonMovableMarker& operator=(const NonCopyableNonMovableMarker&) = delete;
    NonCopyableNonMovableMarker(NonCopyableNonMovableMarker&&) = delete;
    NonCopyableNonMovableMarker& operator=(NonCopyableNonMovableMarker&&) = delete;
};

template <typename T>
const T& GetRandomElement(const std::vector<T>& vec)
{
    if (vec.empty())
    {
        throw std::runtime_error("Error: Vector is empty.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, vec.size() - 1);

    return vec[distribution(gen)];
}

bool IsRandomNumberLessThanOrEqualTo(int32_t min, int32_t max, int32_t threshold);