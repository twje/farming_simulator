#pragma once

#include <string>

bool IsSubstring(const std::string& source, const std::string& substring);
void ReplaceSubstring(std::string& source, const std::string& substringToReplace, const std::string& newSubstring);
std::string SplitAndGetElement(const std::string& input, char delimiter, int index);
