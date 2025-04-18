#ifndef CHUNK_COORDINATOR_HPP
#define CHUNK_COORDINATOR_HPP

// chunk_coordinator.hpp: Declaration of ChunkCoordinator class for multithreaded chunk processing.
// Coordinates parallel processing of file chunks and manages temporary files.

#include "file_handle.hpp"
#include "parser.hpp"
#include "temp_file.hpp"
#include <memory>
#include <vector>

// ChunkCoordinator: Manages multithreaded processing of file chunks.
// Uses dependency injection for file handle and parser.
class ChunkCoordinator final {
public:
    // Constructor: Initializes with file handle, parser, and file size.
    // Parameters:
    //   input_file: Unique pointer to the input file handle.
    //   parser: Unique pointer to the parser.
    //   file_size: Total size of the input file.
    ChunkCoordinator(std::unique_ptr<FileHandle> input_file, std::unique_ptr<Parser> parser, size_t file_size) noexcept;

    // process_chunks: Splits file into chunks and processes them in parallel.
    // Returns: Vector of TempFile objects for sorted chunks.
    std::vector<TempFile> process_chunks() noexcept;

private:
    std::unique_ptr<FileHandle> input_file_; // File handle for input file.
    std::unique_ptr<Parser> parser_;         // Parser for word extraction.
    size_t file_size_;                       // Total size of the input file.
};

#endif // CHUNK_COORDINATOR_HPP
