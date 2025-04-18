// test_file_handle.cpp: Unit tests for SyscallFileHandle class.
// Validates basic file operations: open, write, read, seek, and cleanup.

#include "file_handle.hpp"
#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <fstream>

// Helper function: creates a test file with given content.
void create_test_file(const std::string& name, const std::string& content) {
    std::ofstream out(name);
    out << content;
    out.close();
}

// Test: Opening a valid file should result in a valid descriptor.
TEST(FileHandleTest, OpensFile) {
    std::string filename = "test_file.txt";
    create_test_file(filename, "hello");

    SyscallFileHandle handle(filename.c_str(), O_RDONLY);
    EXPECT_TRUE(handle.is_open());

    unlink(filename.c_str());
}

// Test: Read operation should return correct content.
TEST(FileHandleTest, ReadsFileContent) {
    std::string filename = "test_file.txt";
    create_test_file(filename, "abc");

    SyscallFileHandle handle(filename.c_str(), O_RDONLY);
    char buf[4] = {0};
    ssize_t n = handle.read(buf, 3);
    EXPECT_EQ(n, 3);
    EXPECT_STREQ(buf, "abc");

    unlink(filename.c_str());
}

// Test: Writing to a file.
TEST(FileHandleTest, WritesToFile) {
    std::string filename = "write_test.txt";
    SyscallFileHandle handle(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    const char* msg = "test";
    ssize_t n = handle.write(msg, 4);
    EXPECT_EQ(n, 4);

    std::ifstream in(filename);
    std::string content;
    in >> content;
    EXPECT_EQ(content, "test");

    unlink(filename.c_str());
}
