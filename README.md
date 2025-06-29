# AOS-MyGit - Mini Version Control System

## Overview
This project, `MyGit`, is a mini version control system (VCS) developed as part of the AOS Assignment 4. It mimics the core functionalities of Git, allowing users to manage file versions, track changes, and revert to previous states.

## Project Structure
```
AOS-MyGit/
├── LICENSE
├── Makefile
├── README.md
├── demo.sh
├── test_script.sh
├── src/
│   ├── main.cpp
│   ├── mygit.cpp
│   ├── mygit.h
│   ├── sha1.cpp
│   ├── sha1.h
│   ├── ui_utils.cpp
│   ├── ui_utils.h
│   ├── utils.cpp
│   └── utils.h
```

## Installation and Execution

### Prerequisites
- A C++17-compatible compiler (e.g., `g++`).
- The `zlib` library for compression (installed via `sudo apt-get install zlib1g-dev` on Ubuntu or equivalent).
- Bash shell for running test and demo scripts.

### Build Instructions
1. Navigate to the project directory:
   ```bash
   cd AOS-MyGit
   ```
2. Compile the project using the provided Makefile:
   ```bash
   make
   ```
   This generates the executable `mygit`.
3. (Optional) Install the executable:
   ```bash
   make install
   ```

### Running the Program
- Execute the `mygit` binary with various commands (see Usage below).
- Example:
  ```bash
  ./mygit init
  ```

### Running Tests
- Execute the test script to verify functionality:
  ```bash
  chmod +x test_script.sh
  ./test_script.sh
  ```

### Running the Demo
- Run the interactive demo:
  ```bash
  chmod +x demo.sh
  ./demo.sh
  ```

### Cleanup
- Remove compiled files and test artifacts:
  ```bash
  make clean
  ```

## Working Procedure
MyGit operates by maintaining a repository in a `.mygit` directory, which stores objects (blobs, trees, and commits) compressed with zlib and hashed using SHA-1. The system tracks file changes via an index file and updates the `HEAD` to reflect the current commit. Key steps include:
- **Initialization**: Creates the `.mygit` directory structure.
- **Staging**: Adds files to the index for the next commit.
- **Committing**: Creates a snapshot with a unique SHA-1 hash.
- **History and Restoration**: Logs commits and checks out previous states.

## Assumptions
- The working directory is the parent of `.mygit` when commands are executed.
- File paths are relative to the current directory.
- Compression failures default to storing uncompressed data.
- Invalid commands or non-existent files are handled with error messages.
- The system assumes a single branch (`master`) for simplicity.
- Timestamps are generated using local system time.

## Implemented Features

### 1. Initialize Repository
- **Command**: `./mygit init`
- **Description**: Initializes a new repository by creating the `.mygit` directory with subdirectories for objects, refs, and files like `HEAD` and `index`.
- **Output**: Success message and directory structure confirmation.

### 2. Hash Object
- **Command**: `./mygit hash-object [-w] <file>`
- **Description**: Computes the SHA-1 hash of a file. With `-w`, stores it as a blob object.
- **Output**: Displays the SHA-1 hash (e.g., `a3c9c2f58c4b6a2e5f0f48757c2f0e4ef9b8d6d4`).

### 3. Cat-File
- **Command**: `./mygit cat-file <flag> <hash>`
- **Description**: Displays object content or metadata.
  - `-p`: Prints content.
  - `-s`: Shows size.
  - `-t`: Shows type (e.g., `blob`).
- **Output**: Varies by flag (e.g., file content for `-p`).

### 4. Write Tree
- **Command**: `./mygit write-tree`
- **Description**: Creates a tree object from the current directory structure.
- **Output**: SHA-1 hash of the tree (e.g., `a3c9c2f5bc4b6a2e5f01448757c2f0e4ef9b6d6d4`).

### 5. List Tree (ls-tree)
- **Command**: `./mygit ls-tree [--name-only] <hash>`
- **Description**: Lists tree contents.
  - Without `--name-only`: Shows mode, type, hash, and name.
  - With `--name-only`: Lists only names.
- **Output**: Detailed or name-only list (e.g., `100644 blob ef... README.md`).

### 6. Add Files
- **Command**: `./mygit add <file1> <file2> | .`
- **Description**: Stages files or all files (`.`) in the index.
- **Output**: Success messages for each added file.

### 7. Commit Changes
- **Command**: `./mygit commit [-m "message"]`
- **Description**: Creates a commit with an optional message; updates `HEAD`.
- **Output**: Commit SHA (e.g., `a3c9c2f5b6c4b6ba2e5f0f4b7f5f7c2f0e4ef9b6b6d4`).

### 8. Log Command
- **Command**: `./mygit log`
- **Description**: Displays commit history with SHA, parent, message, timestamp, and author.
- **Output**: Formatted commit log.

### 9. Checkout Command
- **Command**: `./mygit checkout <hash>`
- **Description**: Restores the state to the specified commit.
- **Output**: Success message and commit details.

## Error Handling
- Validates file existence, command syntax, and object hashes.
- Reports errors via colored output (e.g., `Error: File not found`).
- Handles segmentation faults and compilation issues with proper checks.
