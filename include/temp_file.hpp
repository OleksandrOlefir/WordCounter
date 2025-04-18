#ifndef TEMP_FILE_HPP
#define TEMP_FILE_HPP

// temp_file.hpp: Declaration of TempFile class for RAII-based temporary file management.
// Manages temporary files with automatic deletion on destruction.

#include <string>

// TempFile: RAII class for managing temporary files.
// Creates unique file names and deletes files when destroyed.
class TempFile final {
public:
    // Constructor: Creates a temporary file with a unique name.
    TempFile() noexcept;

    // Copy constructor: Deleted to prevent file duplication.
    TempFile(const TempFile&) = delete;

    // Copy assignment: Deleted to prevent file duplication.
    TempFile& operator=(const TempFile&) = delete;

    // Move constructor: Transfers temporary file ownership.
    TempFile(TempFile&& other) noexcept;

    // Move assignment: Transfers temporary file ownership.
    TempFile& operator=(TempFile&& other) noexcept;

    // Destructor: Deletes the temporary file if it exists.
    ~TempFile() noexcept;

    // name: Gets the temporary file name.
    // Returns: Reference to the file name string.
    const std::string& name() const noexcept;

private:
    std::string name_; // Name of the temporary file.
};

#endif // TEMP_FILE_HPP
