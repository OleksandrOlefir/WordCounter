#ifndef CHUNK_PROCESSOR_HPP
#define CHUNK_PROCESSOR_HPP

// chunk_processor.hpp: Declaration of ChunkProcessor class for processing file chunks.
// Reads, parses, sorts, and writes a chunk to a temporary file.

#include "file_handle.hpp"
#include "parser.hpp"
#include <memory>
#include <string>

// ChunkProcessor: Processes a single file chunk in a thread-safe manner.
// Uses dependency injection for file handle and parser.
class ChunkProcessor final {
public:
    // Constructor: Initializes with file handle and parser.
    // Parameters:
    //   input_file: Unique pointer to the input file handle.
    //   parser: Unique pointer to the parser.
    ChunkProcessor(std::unique_ptr<FileHandle> input_file, std::unique_ptr<Parser> parser) noexcept;

    // process: Processes a chunk and writes sorted words to a temporary file.
    // Parameters:
    //   start_offset: Starting offset in the input file.
    //   chunk_size: Size of the chunk.
    //   temp_filename: Name of the temporary file.
    void process(off_t start_offset, size_t chunk_size, const std::string& temp_filename) noexcept;

private:
    std::unique_ptr<FileHandle> input_file_; // File handle for reading input.
    std::unique_ptr<Parser> parser_;         // Parser for word extraction.
};

#endif // CHUNK_PROCESSOR_HPP
