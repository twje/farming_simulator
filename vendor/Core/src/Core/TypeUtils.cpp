#include "Core/TypeUtils.h"

uint32_t GetNextTypeId()
{
    static uint32_t value = 0;
    return value++;
}