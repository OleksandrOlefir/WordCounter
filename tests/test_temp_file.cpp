// test_temp_file.cpp: Tests for the TempFile class.
// Verifies that temporary files are created, moved, and deleted as expected.

#include "temp_file.hpp"
#include <gtest/gtest.h>
#include <unistd.h>
#include <fstream>

// Test: Upon creation, TempFile generates a valid filename.
TEST(TempFileTest, GeneratesUniqueFilename) {
    TempFile tmp;
    EXPECT_FALSE(tmp.name().empty());
}

// Test: The file can be created and written to.
TEST(TempFileTest, FileIsWritable) {
    TempFile tmp;
    std::ofstream out(tmp.name());
    ASSERT_TRUE(out.is_open());
    out << "test content";
    out.close();

    std::ifstream in(tmp.name());
    ASSERT_TRUE(in.is_open());

    std::string content;
    in >> content;
    EXPECT_EQ(content, "test");
}

// Test: Move constructor transfers ownership and clears the original.
TEST(TempFileTest, MoveConstructorTransfersOwnership) {
    TempFile tmp1;
    std::string original_name = tmp1.name();
    TempFile tmp2 = std::move(tmp1);
    EXPECT_EQ(tmp1.name(), "");
    EXPECT_EQ(tmp2.name(), original_name);
}

// Test: Moved-from file is not deleted twice.
TEST(TempFileTest, MoveAssignmentTransfersOwnership) {
    TempFile tmp1;
    std::string name1 = tmp1.name();
    TempFile tmp2;
    tmp2 = std::move(tmp1);
    EXPECT_EQ(tmp1.name(), "");
    EXPECT_EQ(tmp2.name(), name1);
}
