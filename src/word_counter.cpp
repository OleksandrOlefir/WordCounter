// word_counter.cpp: Implementation of WordCounter for counting unique words.
// This file merges sorted temporary files using a priority queue to count unique words.

#include "word_counter.hpp"
#include "file_word.hpp"
#include <queue>
#include <string>

// Constructor: Initializes WordCounter with a file handle.
// Parameters:
//   file_handle: Unique pointer to the file handle (used for validation).
// Uses dependency injection for flexibility.
WordCounter::WordCounter(std::unique_ptr<FileHandle> file_handle) noexcept
    : file_handle_(std::move(file_handle)) {       
}

// count_unique_words: Counts unique words by merging sorted temporary files.
// Parameters:
//   temp_files: Vector of TempFile objects containing sorted words.
// Returns:
//   Number of unique words across all temporary files.
// Uses a priority queue to merge words in sorted order, counting unique occurrences.
size_t WordCounter::count_unique_words(const std::vector<TempFile>& temp_files) noexcept {
    // Priority queue to merge words from multiple files (min-heap).
    std::priority_queue<FileWord, std::vector<FileWord>, std::greater<FileWord>> pq;
    size_t unique_count = 0;
    std::string last_word;

    // Initialize the priority queue with the first word from each temporary file.
    for (const auto& temp_file : temp_files) {
        auto fd = std::make_unique<SyscallFileHandle>(temp_file.name().c_str(), O_RDONLY);
        if (!fd->is_open()) {
            ssize_t res = write(STDERR_FILENO, "Error: Could not open temp file\n", 31);
            (void)res;
            _exit(1);
        }

        // Read the first word from the file.
        std::string word;
        char c;
        // Read until space or newline
        while (fd->read(&c, 1) == 1 && c != ' ' && c != '\n') {
            word += c;
        }

        if (!word.empty()) {
            pq.push({std::move(word), std::move(fd)});
        }
    }

    // Merge words from the priority queue, counting unique words.
    while (!pq.empty()) {
        FileWord file_word = std::move(const_cast<FileWord&>(pq.top()));
        pq.pop();        

        // Count unique word if it differs from the last word.
        if (last_word != file_word.word) {
            ++unique_count;
            last_word = file_word.word;
        }

        // Read the next word from the same file.
        std::string next_word;
        char c;
        while (file_word.file->read(&c, 1) == 1 && c != '\n') {
            next_word += c;
        }
        if (!next_word.empty()) {
            // Push the next word and its file handle back to the priority queue.
            pq.push({std::move(next_word), std::move(file_word.file)});
        }
    }

    return unique_count;
}
