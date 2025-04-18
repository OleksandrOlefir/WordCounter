// test_main.cpp: Smoke test to ensure that Google Test framework is configured correctly.

#include <gtest/gtest.h>

// This test ensures that the test framework is operational.
TEST(SmokeTest, FrameworkWorks) {
    EXPECT_EQ(1, 1);
}
