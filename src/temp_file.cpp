// temp_file.cpp: Implementation of TempFile for RAII-based temporary file management.
// This file creates and deletes temporary files used for sorted chunks.

#include "temp_file.hpp"
#include <sstream>
#include <unistd.h>

// TempFile constructor: Creates a temporary file with a unique name.
// The name format is "temp_chunk_<pid>_<index>.tmp", where index increments globally.
TempFile::TempFile() noexcept {
    // Use process ID and a static counter to generate unique file names.
    static size_t index = 0;
    std::ostringstream oss;
    oss << "temp_chunk_" << getpid() << "_" << index++ << ".tmp";
    name_ = oss.str();
}

// Move constructor: Transfers ownership of the temporary file name.
// Parameters:
//   other: Source TempFile to move from.
// Ensures the source is left in a valid state (empty name).
TempFile::TempFile(TempFile&& other) noexcept
    : name_(std::move(other.name_)) {
    other.name_.clear();
}

// Move assignment operator: Transfers ownership of the temporary file name.
// Parameters:
//   other: Source TempFile to move from.
// Returns:
//   Reference to this TempFile.
// Deletes existing file before taking ownership.
TempFile& TempFile::operator=(TempFile&& other) noexcept {
    if (this != &other) {
        // Delete current temporary file if it exists.
        if (!name_.empty()) {
            ::unlink(name_.c_str());
        }
        // Transfer ownership and reset source.
        name_ = std::move(other.name_);
        other.name_.clear();
    }
    return *this;
}

// Destructor: Deletes the temporary file if it exists.
// Ensures RAII cleanup to prevent disk space leaks.
TempFile::~TempFile() noexcept {
    if (!name_.empty()) {
        ::unlink(name_.c_str());
    }
}

// name: Gets the temporary file name.
// Returns: Reference to the file name string.
const std::string& TempFile::name() const noexcept {
    return name_;
}
