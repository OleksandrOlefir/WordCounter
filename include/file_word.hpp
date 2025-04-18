#ifndef FILE_WORD_HPP
#define FILE_WORD_HPP

// file_word.hpp: Declaration of FileWord struct used in word merging phase.
// Associates a word with its corresponding input file for priority queue merging.

#include <string>
#include <memory>
#include "file_handle.hpp"

// FileWord: Helper struct used during merging sorted temporary files.
// Each FileWord pairs a word with its source file, enabling efficient stream merging.
struct FileWord {
    std::string word;                            // The word extracted from the file.
    std::unique_ptr<FileHandle> file;            // Handle to the file from which the word came.

    // Comparison operator for std::priority_queue (min-heap behavior).
    // Ensures that the lexicographically smallest word is at the top of the heap.
    bool operator>(const FileWord& other) const {
        return word > other.word;
    }
};

#endif // FILE_WORD_HPP
