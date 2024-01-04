#include "Core/Utils.h"

bool IsRandomNumberLessThanOrEqualTo(int32_t min, int32_t max, int32_t threshold)
{
    if (max < min)
    {
        return false;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);

    return distrib(gen) <= threshold;
}