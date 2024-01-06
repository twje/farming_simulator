#include "Core/Support.h"

#include <fstream>
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

std::string ReadFile(const std::string& filepath)
{
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (!in)  // checks both fail and bad state
    {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    in.seekg(0, std::ios::end);
    std::streampos size = in.tellg();
    if (size == -1)
    {
        throw std::runtime_error("Error determining the size of the file: " + filepath);
    }

    std::string contents;
    contents.resize(size);
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], size);
    
    if (in.fail() && !in.eof())
    {
        throw std::runtime_error("Error occurred while reading the file: " + filepath);
    }

    return contents;
}

void Trim(std::string& value)
{
    size_t start = value.find_first_not_of(" \t\r\n");
    size_t end = value.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos)
    {
        value.clear(); // String contains only whitespace
    }
    else
    {
        value = value.substr(start, end - start + 1);
    }
}

size_t FindNextLine(const std::string& source, size_t pos)
{
    size_t nextPos = source.find_first_not_of("\r\n", pos);
    if (nextPos == std::string::npos)
    {
        throw std::runtime_error("Unable to find start of next line.");
    }
    return nextPos;
}