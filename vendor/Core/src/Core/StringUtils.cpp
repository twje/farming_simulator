#include "Core/StringUtils.h"

#include <iostream>
#include <cctype>

std::string CapitalizeString(const std::string& word)
{
    if (word.empty())
    {
        return word;
    }

    std::string result = word;
    result[0] = std::toupper(result[0]);
    return result;
}