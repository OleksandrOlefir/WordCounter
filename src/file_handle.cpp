// file_handle.cpp: Implementation of SyscallFileHandle for RAII-based file operations.
// This file provides a safe interface for Linux syscalls (open, read, write, seek, close).

#include "file_handle.hpp"
#include <fcntl.h>
#include <unistd.h>

// SyscallFileHandle default constructor: Initializes with invalid file descriptor.
SyscallFileHandle::SyscallFileHandle() noexcept
    : fd_(-1) {
}

// SyscallFileHandle constructor: Opens a file using syscall open.
// Parameters:
//   filename: Path to the file.
//   flags: Open flags (e.g., O_RDONLY, O_WRONLY).
// The file descriptor is initialized to -1 if open fails.
SyscallFileHandle::SyscallFileHandle(const char* filename, int flags) noexcept
    : fd_(::open(filename, flags)) {
}

// SyscallFileHandle constructor: Opens a file with mode using syscall open.
// Parameters:
//   filename: Path to the file.
//   flags: Open flags (e.g., O_WRONLY | O_CREAT).
//   mode: File permissions (e.g., 0600 for owner read/write).
// The file descriptor is initialized to -1 if open fails.
SyscallFileHandle::SyscallFileHandle(const char* filename, int flags, mode_t mode) noexcept
    : fd_(::open(filename, flags, mode)) {
}

// SyscallFileHandle constructor: Takes ownership of an existing file descriptor.
// Parameters:
//   fd: Existing file descriptor to manage.
// Assumes the descriptor is valid; does not close it on construction.
SyscallFileHandle::SyscallFileHandle(int fd) noexcept
    : fd_(fd) {
}

// Move constructor: Transfers ownership of the file descriptor.
// Parameters:
//   other: Source SyscallFileHandle to move from.
// Ensures the source is left in a valid state (fd_ = -1).
SyscallFileHandle::SyscallFileHandle(SyscallFileHandle&& other) noexcept
    : fd_(other.fd_) {
    other.fd_ = -1;
}

// Move assignment operator: Transfers ownership of the file descriptor.
// Parameters:
//   other: Source SyscallFileHandle to move from.
// Returns:
//   Reference to this SyscallFileHandle.
// Closes existing file descriptor before taking ownership.
SyscallFileHandle& SyscallFileHandle::operator=(SyscallFileHandle&& other) noexcept {
    if (this != &other) {
        // Close current file descriptor if open.
        if (fd_ != -1) {
            ::close(fd_);
        }
        // Transfer ownership and reset source.
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

// Destructor: Closes the file descriptor if open.
// Ensures RAII cleanup to prevent resource leaks.
SyscallFileHandle::~SyscallFileHandle() noexcept {
    if (fd_ != -1) {
        ::close(fd_);
    }
}

// is_open: Checks if the file descriptor is valid.
// Returns:
//   True if the file is open (fd_ != -1), false otherwise.
bool SyscallFileHandle::is_open() const noexcept {
    return fd_ != -1;
}

// get: Retrieves the file descriptor.
// Returns:
//   The file descriptor (fd_).
int SyscallFileHandle::get() const noexcept {
    return fd_;
}

// seek: Sets the file offset using lseek syscall.
// Parameters:
//   offset: New offset value.
//   whence: Seek mode (e.g., SEEK_SET, SEEK_CUR, SEEK_END).
// Returns:
//   The resulting offset, or -1 on error.
off_t SyscallFileHandle::seek(off_t offset, int whence) noexcept {
    return ::lseek(fd_, offset, whence);
}

// read: Reads data from the file into a buffer using read syscall.
// Parameters:
//   buffer: Destination buffer for read data.
//   size: Number of bytes to read.
// Returns:
//   Number of bytes read, or -1 on error.
ssize_t SyscallFileHandle::read(char* buffer, size_t size) noexcept {
    return ::read(fd_, buffer, size);
}

// write: Writes data from a buffer to the file using write syscall.
// Parameters:
//   buffer: Source buffer containing data to write.
//   size: Number of bytes to write.
// Returns:
//   Number of bytes written, or -1 on error.
ssize_t SyscallFileHandle::write(const char* buffer, size_t size) noexcept {
    return ::write(fd_, buffer, size);
}
