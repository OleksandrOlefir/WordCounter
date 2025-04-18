// parser.cpp: Implementation of SpaceSeparatedParser for parsing input into words.
// This file processes input buffers, splitting them into space-separated words.

#include "parser.hpp"

// parse: Splits a buffer into words based on spaces and stores them in a vector.
// Parameters:
//   buffer: Input buffer containing lowercase letters and spaces.
//   size: Size of the buffer.
//   words: Output vector to store parsed words.
// Assumes input is valid (lowercase 'a' to 'z', spaces) per project requirements.
void SpaceSeparatedParser::parse(const char* buffer, size_t size, std::vector<std::string>& words) noexcept {
    std::string current_word;
    // Iterate through each character in the buffer.
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == ' ') {
            // End of a word: add to vector if non-empty.
            if (!current_word.empty()) {
                words.push_back(std::move(current_word));
                current_word.clear();
            }
        } else {
            // Append non-space character to current word.
            current_word += buffer[i];
        }
    }
    // Add the last word if non-empty.
    if (!current_word.empty()) {
        words.push_back(std::move(current_word));
    }
}
