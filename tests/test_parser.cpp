// test_parser.cpp: Unit tests for the SpaceSeparatedParser class.
// Verifies that the parser correctly splits ASCII lowercase strings into words based on spaces.

#include "parser.hpp"
#include <gtest/gtest.h>

// Test: Empty string should result in an empty list of words.
TEST(ParserTest, ParsesEmptyString) {
    SpaceSeparatedParser parser;
    std::vector<std::string> words;
    parser.parse("", 0, words);
    EXPECT_TRUE(words.empty());
}

// Test: A single word without spaces should be parsed correctly.
TEST(ParserTest, ParsesSingleWord) {
    SpaceSeparatedParser parser;
    std::vector<std::string> words;
    parser.parse("hello", 5, words);
    ASSERT_EQ(words.size(), 1);
    EXPECT_EQ(words[0], "hello");
}

// Test: Multiple words separated by spaces.
TEST(ParserTest, ParsesMultipleWords) {
    SpaceSeparatedParser parser;
    std::vector<std::string> words;
    parser.parse("one two three", 13, words);
    ASSERT_EQ(words.size(), 3);
    EXPECT_EQ(words[0], "one");
    EXPECT_EQ(words[1], "two");
    EXPECT_EQ(words[2], "three");
}

// Test: Handles leading, trailing, and multiple consecutive spaces correctly.
TEST(ParserTest, HandlesExtraSpaces) {
    SpaceSeparatedParser parser;
    std::vector<std::string> words;
    parser.parse("  hello   world  ", 17, words);
    ASSERT_EQ(words.size(), 2);
    EXPECT_EQ(words[0], "hello");
    EXPECT_EQ(words[1], "world");
}
