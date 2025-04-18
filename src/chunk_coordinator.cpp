// chunk_coordinator.cpp: Implementation of ChunkCoordinator for multithreaded chunk processing.
// This file splits the input file into chunks, processes them in parallel, and manages temporary files.

#include "chunk_coordinator.hpp"
#include "chunk_processor.hpp"
#include <thread>
#include <mutex>

// Global mutex and offset for thread-safe chunk assignment.
static std::mutex offset_mutex;
static off_t global_offset = 0;

// Constructor: Initializes ChunkCoordinator with file handle, parser, and file size.
// Parameters:
//   input_file: Unique pointer to the input file handle.
//   parser: Unique pointer to the parser for word extraction.
//   file_size: Total size of the input file.
// Uses dependency injection for flexibility.
ChunkCoordinator::ChunkCoordinator(std::unique_ptr<FileHandle> input_file, std::unique_ptr<Parser> parser, size_t file_size) noexcept
    : input_file_(std::move(input_file)), parser_(std::move(parser)), file_size_(file_size) {
}

// process_chunks: Splits the file into chunks and processes them in parallel.
// Returns:
//   Vector of TempFile objects representing sorted chunk files.
// Uses a thread pool to parallelize chunk processing, ensuring thread safety.
std::vector<TempFile> ChunkCoordinator::process_chunks() noexcept {
    std::vector<TempFile> temp_files;
    std::vector<std::thread> threads;
    // Chunk size is 1 GiB to balance memory usage and parallelism.
    constexpr size_t CHUNK_SIZE = 1ULL << 30;
    size_t num_chunks = (file_size_ + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // Reset global offset for chunk assignment.
    global_offset = 0;

    // Create threads up to hardware concurrency for optimal performance.
    size_t max_threads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < num_chunks; ++i) {
        // Create a new temporary file for the chunk.
        TempFile temp_file;
        temp_files.push_back(std::move(temp_file));

        // Assign chunk offset in a thread-safe manner.
        off_t start_offset;
        {
            std::lock_guard<std::mutex> lock(offset_mutex);
            start_offset = global_offset;
            global_offset += CHUNK_SIZE;
        }

        // Create a new processor for the chunk, sharing the file descriptor.
        auto processor = std::make_unique<ChunkProcessor>(
            // Use existing file descriptor to avoid reopening the file.
            std::make_unique<SyscallFileHandle>(input_file_->get()),
            std::make_unique<SpaceSeparatedParser>()
        );

        // Launch thread to process the chunk.
        threads.emplace_back([processor = std::move(processor), start_offset, chunk_size = CHUNK_SIZE, temp_filename = temp_files.back().name()]() {
            processor->process(start_offset, chunk_size, temp_filename);
        });

        // Limit active threads to avoid resource exhaustion.
        if (threads.size() >= max_threads) {
            for (auto& t : threads) {
                t.join();
            }
            threads.clear();
        }
    }
    // Join remaining threads to ensure all chunks are processed.
    for (auto& t : threads) {
        t.join();
    }

    return temp_files;
}
