#pragma once

#include <cstdint>

uint32_t GetNextTypeId();

template<typename T>
struct TypeId
{
    static uint32_t Get()
    {
        static uint32_t typeId = GetNextTypeId();
        return typeId;
    }
};