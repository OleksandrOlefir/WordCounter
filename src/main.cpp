// main.cpp: Entry point for the word counter program.
// This file validates command-line arguments, initializes components,
// and orchestrates the workflow to count unique words in a large file.

#include "chunk_coordinator.hpp"
#include "file_handle.hpp"
#include "parser.hpp"
#include "word_counter.hpp"
#include <memory>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

// Main function: Validates input, sets up components, and executes the word-counting process.
// Parameters:
//   argc: Number of command-line arguments.
//   argv: Array of command-line argument strings (expects argv[1] as input file name).
// Returns:
//   0 on success, 1 on error (invalid arguments, file access issues).
int main(int argc, char* argv[]) {
    // Validate command-line arguments: expect exactly one input file name.
    if (argc != 2) {
        ssize_t res = write(STDERR_FILENO, "Usage: ", 7);
        res = write(STDERR_FILENO, argv[0], strlen(argv[0]));
        res = write(STDERR_FILENO, " <filename>\n", 12);
        (void)res;
        return 1;
    }

    // Check if the input file exists and is accessible using stat.
    struct stat st;
    if (stat(argv[1], &st) == -1) {
        ssize_t res = write(STDERR_FILENO, "Error: Could not stat file\n", 27);
        (void)res;
        return 1;
    }

    // Open the input file using SyscallFileHandle.
    auto input_file = std::make_unique<SyscallFileHandle>(argv[1], O_RDONLY);
    if (!input_file->is_open()) {
        ssize_t res = write(STDERR_FILENO, "Error: Could not open input file\n", 32);
        (void)res;
        return 1;
    }

    // Initialize the parser for space-separated words.
    auto parser = std::make_unique<SpaceSeparatedParser>();

    // Coordinate chunk processing: splits file into chunks and processes them in parallel.
    ChunkCoordinator coordinator(std::move(input_file), std::move(parser), st.st_size);
    auto temp_files = coordinator.process_chunks();

    // Count unique words by merging sorted temporary files.
    auto word_counter_file = std::make_unique<SyscallFileHandle>(argv[1], O_RDONLY);
    WordCounter counter(std::move(word_counter_file));
    size_t unique_count = counter.count_unique_words(temp_files);

    // Output the count of unique words to stdout.
    std::string count_str = std::to_string(unique_count) + "\n";
    ssize_t res = write(STDOUT_FILENO, count_str.c_str(), count_str.size());
    (void)res;

    return 0;
}
