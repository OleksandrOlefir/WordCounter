#ifndef PARSER_HPP
#define PARSER_HPP

// parser.hpp: Declarations for Parser interface and SpaceSeparatedParser class.
// Provides an abstract interface for parsing input into words and a space-separated implementation.

#include <vector>
#include <string>

// Parser: Abstract interface for parsing input buffers into words.
class Parser {
public:
    // parse: Splits a buffer into words and stores them in a vector.
    // Parameters:
    //   buffer: Input buffer to parse.
    //   size: Size of the buffer.
    //   words: Output vector to store parsed words.
    virtual void parse(const char* buffer, size_t size, std::vector<std::string>& words) = 0;

    // Destructor: Virtual to ensure proper cleanup in derived classes.
    virtual ~Parser() noexcept = default;
};

// SpaceSeparatedParser: Implementation of Parser for space-separated words.
// Parses input assuming lowercase letters and spaces.
class SpaceSeparatedParser : public Parser {
public:
    // parse: Splits buffer into words based on spaces.
    void parse(const char* buffer, size_t size, std::vector<std::string>& words) noexcept override;
};

#endif // PARSER_HPP
