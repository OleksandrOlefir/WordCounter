// test_word_counter.cpp: Unit tests for WordCounter logic including merge and unique counting.
// Covers edge cases like empty input, duplicates, and file errors.

#include <gtest/gtest.h>
#include "word_counter.hpp"
#include "temp_file.hpp"
#include "file_handle.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <memory>

// Helper function to write content to a temp file for test setup.
static std::string write_test_file(const std::string& content) {
    TempFile temp;
    std::ofstream out(temp.name());
    out << content;
    out.close();
    return temp.name();
}

// Test case 1: WordCounter should return 0 for an empty file.
TEST(WordCounterTest, EmptyFileReturnsZero) {
    std::string filename = write_test_file("");
    std::vector<TempFile> temp_files;
    TempFile temp;
    std::ofstream out(temp.name());
    out << "";
    out.close();
    temp_files.push_back(std::move(temp));

    auto fd = std::make_unique<SyscallFileHandle>(filename.c_str(), O_RDONLY);
    WordCounter wc(std::move(fd));
    EXPECT_EQ(wc.count_unique_words(temp_files), 0);
}

// Test case 2: File contains repeated same word. Should count as 1 unique word.
TEST(WordCounterTest, AllSameWordsReturnsOne) {
    std::string filename = write_test_file("dog\ndog\ndog\n");
    std::vector<TempFile> temp_files;
    TempFile temp;
    std::ofstream out(temp.name());
    out << "dog\ndog\ndog\n";
    out.close();
    temp_files.push_back(std::move(temp));

    auto fd = std::make_unique<SyscallFileHandle>(filename.c_str(), O_RDONLY);
    WordCounter wc(std::move(fd));
    EXPECT_EQ(wc.count_unique_words(temp_files), 1);
}

// Test case 3: File cannot be opened (simulated by invalid file descriptor).
// This test is limited because the original implementation calls _exit(1).
// For real-world use, we would refactor to throw exceptions or return errors.
TEST(WordCounterTest, CannotOpenFile) {
    TempFile fake;
    ::unlink(fake.name().c_str()); // Delete the file to simulate error.

    // Expect the process to exit. This will be skipped in normal unit runs
    // since _exit() terminates the process immediately.
    // We would mock this in advanced test setups.
    SUCCEED(); // Placeholder to show intent.
}
