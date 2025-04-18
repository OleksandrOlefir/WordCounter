// chunk_processor.cpp: Implementation of ChunkProcessor for processing file chunks.
// This file reads a chunk, parses it into words, sorts them, and writes to a temporary file.

#include "chunk_processor.hpp"
#include <algorithm>
#include <vector>

#include <iostream>

// Constructor: Initializes ChunkProcessor with file handle and parser.
// Parameters:
//   input_file: Unique pointer to the input file handle.
//   parser: Unique pointer to the parser for word extraction.
// Uses dependency injection for flexibility.
ChunkProcessor::ChunkProcessor(std::unique_ptr<FileHandle> input_file, std::unique_ptr<Parser> parser) noexcept
    : input_file_(std::move(input_file)), parser_(std::move(parser)) {
}

// process: Processes a file chunk and writes sorted words to a temporary file.
// Parameters:
//   start_offset: Starting offset in the input file.
//   chunk_size: Size of the chunk to process.
//   temp_filename: Name of the temporary file to write sorted words.
// Reads the chunk, parses into words, sorts them, and writes to the temporary file.
void ChunkProcessor::process(off_t start_offset, size_t chunk_size, const std::string& temp_filename) noexcept {
    // Set file offset for reading the chunk.
    if (input_file_->seek(start_offset, SEEK_SET) == -1) {
        ssize_t res = write(STDERR_FILENO, "Error: Could not seek in input file\n", 35);
        (void)res;
        return;
    }

    // Buffer for reading the chunk (1 MiB to balance memory and I/O).
    constexpr size_t BUFFER_SIZE = 1ULL << 20;
    std::vector<char> buffer(BUFFER_SIZE);
    size_t total_read = 0;
    std::vector<std::string> words;

    // Read the chunk in smaller buffers to manage memory.
    while (total_read < chunk_size) {
        size_t to_read = std::min(BUFFER_SIZE, chunk_size - total_read);
        ssize_t bytes_read = input_file_->read(buffer.data(), to_read);
        if (bytes_read <= 0) {
            break;
        }
        // Parse the buffer into words.
        parser_->parse(buffer.data(), bytes_read, words);
        total_read += bytes_read;
    }

    // Sort words to prepare for merging.
    std::sort(words.begin(), words.end());

    // Write sorted words to the temporary file.
    SyscallFileHandle temp_file(temp_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (!temp_file.is_open()) {
        ssize_t res = write(STDERR_FILENO, "Error: Could not open temp file for writing\n", 43);
        (void)res;
        return;
    }

    // Write each word followed by a newline.
    for (const auto& word : words) {
        temp_file.write(word.c_str(), word.size());
        temp_file.write("\n", 1);
    }
}
