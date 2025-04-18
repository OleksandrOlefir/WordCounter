#ifndef FILE_HANDLE_HPP
#define FILE_HANDLE_HPP

// file_handle.hpp: Declarations for FileHandle interface and SyscallFileHandle class.
// Provides an abstract interface for file operations and a syscall-based implementation.

#include <string>
#include <unistd.h>
#include <fcntl.h>

// FileHandle: Abstract interface for file operations.
// Defines methods for checking file status, seeking, reading, and writing.
class FileHandle {
public:
    // is_open: Checks if the file is open.
    // Returns: True if the file is open, false otherwise.
    virtual bool is_open() const = 0;

    // get: Retrieves the file descriptor.
    // Returns: The file descriptor.
    virtual int get() const = 0;

    // seek: Sets the file offset.
    // Parameters:
    //   offset: New offset value.
    //   whence: Seek mode (e.g., SEEK_SET).
    // Returns: Resulting offset, or -1 on error.
    virtual off_t seek(off_t offset, int whence) = 0;

    // read: Reads data from the file.
    // Parameters:
    //   buffer: Destination buffer.
    //   size: Number of bytes to read.
    // Returns: Number of bytes read, or -1 on error.
    virtual ssize_t read(char* buffer, size_t size) = 0;

    // write: Writes data to the file.
    // Parameters:
    //   buffer: Source buffer.
    //   size: Number of bytes to write.
    // Returns: Number of bytes written, or -1 on error.
    virtual ssize_t write(const char* buffer, size_t size) = 0;

    // Destructor: Virtual to ensure proper cleanup in derived classes.
    virtual ~FileHandle() noexcept = default;
};

// SyscallFileHandle: RAII-compliant implementation of FileHandle using Linux syscalls.
// Manages file descriptors with automatic closure on destruction.
class SyscallFileHandle : public FileHandle {
public:
    // Default constructor: Initializes with invalid file descriptor.
    SyscallFileHandle() noexcept;

    // Constructor: Opens a file using syscall open.
    // Parameters:
    //   filename: Path to the file.
    //   flags: Open flags (e.g., O_RDONLY).
    SyscallFileHandle(const char* filename, int flags) noexcept;

    // Constructor: Opens a file with mode for creation using syscall open.
    // Parameters:
    //   filename: Path to the file.
    //   flags: Open flags (e.g., O_WRONLY | O_CREAT).
    //   mode: File permissions (e.g., 0600 for owner read/write).
    SyscallFileHandle(const char* filename, int flags, mode_t mode) noexcept;

    // Constructor: Takes ownership of an existing file descriptor.
    // Parameters:
    //   fd: Existing file descriptor to manage.
    explicit SyscallFileHandle(int fd) noexcept;

    // Copy constructor: Deleted to prevent file descriptor duplication.
    SyscallFileHandle(const SyscallFileHandle&) = delete;

    // Copy assignment: Deleted to prevent file descriptor duplication.
    SyscallFileHandle& operator=(const SyscallFileHandle&) = delete;

    // Move constructor: Transfers file descriptor ownership.
    SyscallFileHandle(SyscallFileHandle&& other) noexcept;

    // Move assignment: Transfers file descriptor ownership.
    SyscallFileHandle& operator=(SyscallFileHandle&& other) noexcept;

    // Destructor: Closes the file descriptor if open.
    ~SyscallFileHandle() noexcept;

    // Implement FileHandle interface methods.
    bool is_open() const noexcept override;
    int get() const noexcept override;
    off_t seek(off_t offset, int whence) noexcept override;
    ssize_t read(char* buffer, size_t size) noexcept override;
    ssize_t write(const char* buffer, size_t size) noexcept override;

private:
    int fd_; // File descriptor managed by the class.
};

#endif // FILE_HANDLE_HPP
