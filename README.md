# Word Counter

This C++ program counts the number of unique words in a large text file, designed to handle inputs larger than available RAM (e.g., 32 GiB). The input file must contain only lowercase letters 'a' to 'z' and spaces in ASCII, with words separated by spaces. The program takes the file name as a command-line argument and outputs a single number: the count of unique words. For example, for the input "a horse and a dog", it outputs `4` (unique words: "a", "and", "dog", "horse").

## How to Build and Run the Project

### Prerequisites
- A Linux system (due to syscall usage).
- CMake 3.10 or higher.
- A C++17-compatible compiler (e.g., g++).
- Standard C++ library and POSIX threads (`pthread`).

### Directory Structure
```
WordCounter/
├── src/
│   ├── main.cpp
│   ├── file_handle.cpp
│   ├── temp_file.cpp
│   ├── parser.cpp
│   ├── chunk_processor.cpp
│   ├── chunk_coordinator.cpp
│   ├── word_counter.cpp
├── include/
│   ├── file_handle.hpp
│   ├── file_word.hpp
│   ├── temp_file.hpp
│   ├── parser.hpp
│   ├── chunk_processor.hpp
│   ├── chunk_coordinator.hpp
│   ├── word_counter.hpp
├── CMakeLists.txt
├── README.md
├── TestDataGeneration/
│   ├── generate_test.py      # Python script for generating input test files
├── tests/
│   ├── test_main.cpp
│   ├── test_parser.cpp
│   ├── test_temp_file.
│   ├── test_word_counter
└── ├── test_file_handle.cpp

```

#### File Purposes
- **src/main.cpp**: Serves as the program’s entry point, validating command-line arguments, initializing the file handle, parser, and coordinator, and orchestrating the workflow to process chunks and count unique words.
- **src/file_handle.cpp**: Implements the `SyscallFileHandle` class, providing RAII-compliant file operations (open, read, write, seek, close) using Linux syscalls for efficient file access.
- **src/temp_file.cpp**: Implements the `TempFile` class, managing temporary files for sorted chunks with automatic deletion via RAII to prevent resource leaks.
- **src/parser.cpp**: Implements the `SpaceSeparatedParser` class, parsing input buffers into words based on space separation for chunk processing.
- **src/chunk_processor.cpp**: Implements the `ChunkProcessor` class, reading a file chunk, parsing it into words, sorting them, and writing to a temporary file in a thread-safe manner.
- **src/chunk_coordinator.cpp**: Implements the `ChunkCoordinator` class, managing multithreaded processing of file chunks and coordinating temporary file creation.
- **src/word_counter.cpp**: Implements the `WordCounter` class, merging sorted temporary files using a priority queue to count unique words efficiently.
- **include/file_handle.hpp**: Declares the `FileHandle` abstract interface and `SyscallFileHandle` class for syscall-based file operations.
- **include/file_word.hpp**: Declares the `FileWord` struct used in the merge phase to pair words with file handles during priority queue-based merging in `WordCounter`.
- **include/temp_file.hpp**: Declares the `TempFile` class for managing temporary files with RAII.
- **include/parser.hpp**: Declares the `Parser` abstract interface and `SpaceSeparatedParser` class for parsing input into words.
- **include/chunk_processor.hpp**: Declares the `ChunkProcessor` class for processing individual file chunks.
- **include/chunk_coordinator.hpp**: Declares the `ChunkCoordinator` class for coordinating multithreaded chunk processing.
- **include/word_counter.hpp**: Declares the `WordCounter` class for counting unique words from temporary files.
- **CMakeLists.txt**: Configures the CMake build system, specifying source files, include directories, compiler settings, and threading dependencies.
- **README.md**: Documents the project, including build and run instructions, techniques used, standards compliance, and file purposes.

### Build Instrinput_1gb_1000K_uniq.txtuctions
1. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```
2. **Run CMake** to configure the project:
   ```bash
   cmake ..
   ```
3. **Compile the project**:
   ```bash
   make
   ```
   This generates the `word_counter` executable in the `build` directory.

### Run Instructions
1. **Prepare an input file**:
   Ensure the input file (e.g., `input.txt`) contains only lowercase letters 'a' to 'z' and spaces, with words separated by spaces.
2. **Run the program**:
   ```bash
   ./word_counter input.txt
   ```
   Example:
   - If `input.txt` contains "a horse and a dog", the output will be:
     ```
     4
     ```
    To evaluate performance and memory usage:
    ```bash
    /usr/bin/time -v ./word_counter input.txt
    ```

### Notes
- The program expects exactly one command-line argument (the input file name). If incorrect arguments are provided, it outputs an error message to stderr and exits.
- Temporary files are created during execution and automatically deleted upon completion.

## Techniques Used and Why the Solution Works

The solution is designed to efficiently process large files while adhering to modern C++ practices and specific constraints (C/C++ standard library, Linux syscalls, RAII, SOLID principles, and the Rule of Five). Below are the key techniques and their rationale:

### 1. External Sorting
- **Technique**: The program uses an external sorting approach to handle files larger than RAM:
  - The input file is divided into fixed-size chunks (1 GiB each), small enough to fit in memory.
  - Each chunk is read, parsed into words, sorted in-memory, and written to a temporary file.
  - Sorted temporary files are merged using a priority queue to count unique words in a single pass.
  - During the merge phase, a custom struct FileWord wraps a word and its file handle for the priority queue, enabling efficient merging of sorted streams from temporary files.
- **Why It Works**:
  - Splitting into chunks ensures memory usage remains bounded, regardless of file size.
  - Sorting chunks individually reduces the problem to manageable pieces.
  - The merge phase processes words in sorted order, allowing efficient counting of unique words by comparing adjacent words, minimizing memory and I/O overhead.
  - This approach scales to files much larger than RAM (e.g., 32 GiB), as only one chunk is loaded into memory at a time, and the merge phase streams data from disk.

### 2. Multithreading
- **Technique**: The program parallelizes chunk processing using C++ standard library threads (`std::thread`):
  - A thread pool processes chunks concurrently, with the number of threads limited to the hardware concurrency (e.g., CPU cores).
  - A global mutex (`std::mutex` with `std::lock_guard`) synchronizes access to a shared offset counter for assigning chunks to threads.
- **Why It Works**:
  - Multithreading leverages multiple CPU cores to process chunks in parallel, significantly reducing execution time for large files.
  - Synchronization ensures threads access shared resources safely without race conditions.
  - The thread pool approach balances load and avoids excessive thread creation, optimizing performance.

### 3. RAII (Resource Acquisition Is Initialization)
- **Technique**: All resources are managed using RAII principles:
  - `SyscallFileHandle`: Wraps file descriptors, closing them in the destructor.
  - `TempFile`: Manages temporary file names, deleting files with `unlink` in the destructor.
  - `std::unique_ptr`: Used for dependency injection in `ChunkProcessor`, `ChunkCoordinator`, and `WordCounter`.
  - `std::vector` and `std::string`: Handle dynamic memory automatically.
  - `std::thread`: Threads are joined when their containing `std::vector` goes out of scope.
- **Why It Works**:
  - RAII ensures resources (file descriptors, temporary files, memory, threads) are released automatically, even on errors, preventing leaks.
  - This guarantees robust resource management, critical for processing large files where many temporary files and file descriptors are used.

### 4. SOLID Principles
- **Technique**: The codebase adheres to SOLID principles:
  - **Single Responsibility Principle (SRP)**: Each class has one responsibility (e.g., `SyscallFileHandle` for file operations, `SpaceSeparatedParser` for parsing, `WordCounter` for counting).
  - **Open/Closed Principle (OCP)**: Interfaces like `FileHandle` and `Parser` allow extensions (e.g., new file handle types or parsers) without modifying existing code.
  - **Liskov Substitution Principle (LSP)**: Derived classes (`SyscallFileHandle`, `SpaceSeparatedParser`) can substitute their base interfaces.
  - **Interface Segregation Principle (ISP)**: Interfaces are minimal, avoiding unnecessary methods.
  - **Dependency Inversion Principle (DIP)**: High-level modules depend on abstractions (e.g., `ChunkProcessor` uses `FileHandle` and `Parser` interfaces), injected via `std::unique_ptr`.
- **Why It Works**:
  - SOLID principles ensure a modular, maintainable, and extensible design.
  - Clear separation of concerns reduces bugs and simplifies testing.
  - Dependency injection makes the code flexible for future changes (e.g., supporting different file formats).

### 5. Rule of Five
- **Technique**: Classes managing resources explicitly define or delete the five special member functions (destructor, copy/move constructors, copy/move assignment operators):
  - `SyscallFileHandle` and `TempFile`: Define destructor and move operations, delete copy operations to prevent unsafe duplication.
  - `ChunkProcessor`, `ChunkCoordinator`, `WordCounter`: Use `std::unique_ptr`, relying on compiler-generated defaults (copy deleted, move correct).
  - `SpaceSeparatedParser`: Stateless, uses default member functions.
- **Why It Works**:
  - Explicit management prevents resource leaks or double-free errors.
  - Deleting copy operations for `SyscallFileHandle` and `TempFile` ensures file descriptors and temporary files are handled safely.
  - Compiler-generated defaults for `std::unique_ptr`-based classes are correct, simplifying code while maintaining safety.

### 6. Linux Syscalls Instead of std::fstream
- **Technique**: File operations use Linux syscalls (`open`, `read`, `write`, `lseek`, `close`, `unlink`, `stat`) via `SyscallFileHandle`, explicitly avoiding high-level I/O libraries like `std::fstream`.
- **Why It Works**:
  - **Low-Level Control**: Syscalls provide direct access to file operations, allowing precise control over buffer sizes and file offsets, which is critical for efficiently processing large files (e.g., 32 GiB).
  - **Performance**: Syscalls avoid the buffering and abstraction overhead of `std::fstream`, reducing CPU and memory usage during I/O operations.
  - **Requirement Compliance**: The project explicitly requires the use of Linux syscalls, ensuring compatibility with the specified environment and avoiding dependencies on C++ standard library I/O abstractions.
  - **Safety**: The `SyscallFileHandle` class wraps syscalls in an RAII-compliant interface, ensuring file descriptors are closed automatically and errors are handled securely, maintaining robustness without `std::fstream`.

### 7. Input Validation and Error Handling
- **Technique**:
  - Validates command-line arguments (exactly one file name).
  - Checks file existence and accessibility using `stat` and `open`.
  - Reports errors to `STDERR_FILENO` using `write` (e.g., "Error: Could not open input file").
  - Assumes input adheres to the format (lowercase 'a' to 'z', spaces) for parsing simplicity.
- **Why It Works**:
  - Robust error handling ensures the program fails gracefully with clear messages.
  - The input format guarantee simplifies parsing, focusing on performance for valid inputs.
  - Using `write` for errors aligns with syscall usage, avoiding buffered I/O.

### Why the Solution Succeeds
The combination of these techniques ensures the solution is correct, efficient, and robust:
- **Correctness**: The external sorting algorithm guarantees all words are processed in order, counting each unique word exactly once, verified by the example ("a horse and a dog" yields `4`).
- **Efficiency**: Multithreading and chunked processing minimize runtime, while syscalls and bounded memory usage handle large files without exhausting RAM.
- **Scalability**: The design scales to 32 GiB and beyond, as only one chunk is in memory at a time, and temporary files are managed efficiently.
- **Maintainability**: SOLID principles and RAII make the code modular and safe, easing future extensions.
- **Reliability**: RAII and the Rule of Five ensure no resource leaks, even under errors, while error handling provides clear diagnostics.

## Standards Compliance
The project is designed to adhere to modern C++ standards and secure coding practices, ensuring safety, reliability, and maintainability. Below is a summary of compliance with key standards:

- **C++ Core Guidelines**:
  - The program is highly compliant, leveraging modern C++17 features such as `std::unique_ptr`, `std::vector`, `std::thread`, and `std::mutex`. It follows RAII (R.1), uses `override` for virtual functions (C.128), applies `final` to non-inheritable classes (C.129), and marks non-throwing functions with `noexcept` (E.12, C.66). The use of Linux syscalls is a minor deviation from portability (P.2), justified by the project’s requirements.
- **MISRA C++:2008**:
  - The program is moderately compliant, adhering to rules on initialization (7-1-1), resource management (18-4-1), and avoiding exceptions (15-0-1). It uses C++17 features (e.g., `std::unique_ptr`, `std::thread`) not covered by MISRA’s C++03 basis, and the required use of POSIX syscalls violates Rule 16-2-1 (C input.txtPP) is a minor issue, mitigated by the single-user context.
- **Secure Coding Practices for C++**:
  - The program follows secure coding principles, using RAII to prevent resource leaks, avoiding raw pointers for ownership, and implementing thread-safe concurrency with `std::mutex` and `std::lock_guard`. It validates inputs (command-line arguments, file accessibility) and handles errors securely without exceptions, aligning with safety-critical requirements. The decision to use syscalls instead of `std::fstream` enhances performance and control, contributing to security by minimizing dependencies.

This compliance ensures the program is robust, secure, and suitable for processing large files in a high-performance, single-user environment.

## Test Data Generation

To facilitate benchmarking, stress testing, and validation, this project includes a flexible test data generator in the TestDataGeneration/ directory. It allows you to create input files with configurable size and content — ranging from compact examples with controlled uniqueness to massive files up to 32 GiB.

### This script provides 5 test scenarios, selectable via the --case flag.

### Available Test Scenarios

| Case | Description                                                             |
|------|-------------------------------------------------------------------------|
| 1    | 1,000,000 words randomly chosen from ~1,000 unique words.               |
| 2    | ~1 GiB file with random words and ~100K unique words.                   |
| 3    | ~10 GiB file with random words and ~1M unique words.                    |
| 4    | ~20 GiB file with random words and ~2M unique words.                    |
| 5    | ~32 GiB file with random words and ~5M unique words.                    |

  Each output file consists of space-separated words using only lowercase ASCII letters a–z, conforming to the required input format of the program.

### Prerequisites

Ensure Python 3 and tqdm are installed (for progress bars):
  ```bash
  sudo apt update
  sudo apt install python3-tqdm
  ```

Or, use a virtual environment:
  ```bash
  python3 -m venv venv
  source venv/bin/activate
  pip install tqdm
  ```

### Usage Examples

Navigate to the script directory:

```bash
cd TestDataGeneration
```

Then run one of the following:
Case 1 – 1M words from 1,000 unique words:
```bash
python3 generate_test.py --case 1 --output input_1m_1000uniq.txt
```

Case 2 – ~1 GiB file:
```bash
python3 generate_test.py --case 2 --output input_1gb_100K_uniq.txt
```

Case 3 – ~10 GiB file:
```bash
python3 generate_test.py --case 3 --output input_10gb_1M_uniq.txt
```

Case 4 – ~20 GiB file:
```bash
python3 generate_test.py --case 4 --output input_20gb_2M_uniq.txt
```

Case 5 – ~32 GiB file:
```bash
python3 generate_test.py --case 5 --output input_32gb_5M_uniq.txt
```

### Notes

The generated files are uncompressed and may occupy significant disk space.
Progress bars are displayed during generation.
The output file name is customizable using the --output flag (default is input.txt).
Generated files are compatible with the main program.

## Unit Testing

This project includes a suite of unit tests using Google Test (gtest) to ensure correctness, robustness, and maintainability of core components. The tests cover file handling, parsing, temporary file management, and word counting logic.

### Test Directory Structure

word_counter/
├── tests/
│   ├── test_main.cpp           # Verifies that the test framework is working.
│   ├── test_parser.cpp         # Tests for SpaceSeparatedParser.
│   ├── test_temp_file.cpp      # Tests for TempFile (RAII behavior, file creation/deletion).
│   ├── test_file_handle.cpp    # Tests for SyscallFileHandle (read/write/seek).
│   ├── test_word_counter.          # Integration-like test for WordCounter using small input files.    

### How to Enable and Build Tests

- **Install Google Test (if not available system-wide)**:

```bash
sudo apt install libgtest-dev
sudo apt install cmake g++   # Ensure build tools are available
cd /usr/src/gtest
sudo cmake .
sudo make
cd /usr/src/googletest/googletest # or cd /usr/src/googletest/googletest/build  if cmake created build/ inside
sudo cp lib/*.a /usr/lib
sudo cp *.a /usr/lib         # or /usr/local/lib if needed
```

- **Build with tests: From your build/ directory**:
    ```bash
    cd build
    cmake ..
    make
    ```

- **Run All Tests**

After building:
```bash
./word_counter_tests
```

## Covered Test Cases

### File	Description
- **test_main.cpp	Smoke test to confirm gtest setup works.**
- **test_parser.cpp	Checks correct splitting of text into words, handles edge cases.**
- **test_temp_file.cpp	Ensures temp files are created, moved, and deleted as expected.**
- **test_file_handle.cpp	Validates correct behavior of file open, read, write, and seek.**
- **test_word_counter.cpp	Tests word counting logic on known input (empty, duplicate, invalid file).**
