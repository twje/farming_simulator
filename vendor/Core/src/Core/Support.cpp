#include "Core/Support.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>

bool IsSubstring(const std::string& source, const std::string& substring)
{
	size_t foundPos = source.find(substring);
	return foundPos != std::string::npos;
}

void ReplaceSubstring(std::string& source, const std::string& substringToReplace, const std::string& newSubstring)
{
    size_t pos = source.find(substringToReplace);
    while (pos != std::string::npos)
    {
        source.replace(pos, substringToReplace.length(), newSubstring);
        pos = source.find(substringToReplace, pos + newSubstring.length());
    }
}

std::string SplitAndGetElement(const std::string& input, char delimiter, int index)
{
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(iss, token, delimiter)) 
    {
        tokens.push_back(token);
    }

    if (index >= 0 && index < tokens.size())
    {
        return tokens[index];
    }
    else
    {
        throw std::out_of_range("Index is out of bounds.");
    }
}