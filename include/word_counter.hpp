#ifndef WORD_COUNTER_HPP
#define WORD_COUNTER_HPP

// word_counter.hpp: Declaration of WordCounter class for counting unique words.
// Merges sorted temporary files to count unique words.

#include "file_handle.hpp"
#include "temp_file.hpp"
#include <vector>
#include <memory>

// WordCounter: Counts unique words by merging sorted temporary files.
// Uses a priority queue for efficient merging.
class WordCounter final {
public:
    // Constructor: Initializes with a file handle.
    // Parameters:
    //   file_handle: Unique pointer to the file handle.
    explicit WordCounter(std::unique_ptr<FileHandle> file_handle) noexcept;

    // count_unique_words: Counts unique words from temporary files.
    // Parameters:
    //   temp_files: Vector of TempFile objects with sorted words.
    // Returns: Number of unique words.
    size_t count_unique_words(const std::vector<TempFile>& temp_files) noexcept;

private:
    std::unique_ptr<FileHandle> file_handle_; // File handle for validation.
};

#endif // WORD_COUNTER_HPP
