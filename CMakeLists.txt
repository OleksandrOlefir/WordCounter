cmake_minimum_required(VERSION 3.10)
project(WordCounter LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find threading library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Enable testing
enable_testing()

# Find GTest (from system installation)
find_package(GTest REQUIRED)

# Application sources
set(COMMON_SOURCES
    src/file_handle.cpp
    src/temp_file.cpp
    src/parser.cpp
    src/chunk_processor.cpp
    src/chunk_coordinator.cpp
    src/word_counter.cpp
)

# --- Main executable ---
add_executable(word_counter src/main.cpp ${COMMON_SOURCES})
target_include_directories(word_counter PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Tests executable (without main.cpp) ---
add_executable(word_counter_tests
    src/file_handle.cpp
    src/temp_file.cpp
    src/parser.cpp
    src/chunk_processor.cpp
    src/chunk_coordinator.cpp
    src/word_counter.cpp

    tests/test_main.cpp
    tests/test_parser.cpp
    tests/test_temp_file.cpp
    tests/test_file_handle.cpp
    tests/test_word_counter.cpp
)

target_include_directories(word_counter_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(word_counter_tests
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(word_counter_tests)
