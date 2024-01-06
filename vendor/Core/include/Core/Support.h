#pragma once

#include <string>

bool IsSubstring(const std::string& source, const std::string& substring);
void ReplaceSubstring(std::string& source, const std::string& substringToReplace, const std::string& newSubstring);
std::string SplitAndGetElement(const std::string& input, char delimiter, int index);
std::string ReadFile(const std::string& filepath);


/**
* Trims leading and trailing whitespace from a string.
*
* @param value Reference to the string to be trimmed.
* Usage:
*   std::string example = "   text   ";
*   Trim(example); // example becomes "text"
*/
void Trim(std::string& value);

/**
 * Finds the start of the next line in a string.
 *
 * @param source The string to search.
 * @param pos The starting position for the search.
 * @return Index of the first character of the next line.
 * @throws std::runtime_error if the next line start cannot be found.
 * Usage:
 *   std::string text = "Line 1\nLine 2";
 *   size_t nextLine = FindNextLine(text, 0); // Returns index of 'L' in "Line 2"
 */
size_t FindNextLine(const std::string& source, size_t pos);