# Mini Carver

A low-level, lightweight, command-line data carving tool written in C++ for file
recovery. Mini Carver scans raw disk images, binary dumps or block devices
byte-by-byte, identifying file signatures (headers and footers) to extract lost
artifacts without relying on a file system.

## Features

- Extracts files based on binary signatures.
  Currently supports PDF, JPEG, PNG, and GIF.
- Generates SHA-256 hashes on-the-fly for every extracted file using OpenSSL,
  ensuring chain of custody.
- Implements a sliding window algorithm with a 4MB chunk size and a 16-byte
  overlap, allowing it to process massive disk images without high RAM consumption.
- Safely manages file streams and hash contexts during the extraction state.

## Dependencies

- C++17 compatible compiler (GCC, Clang)
- CMake 3.10 or higher
- OpenSSL (libcrypto)

On Linux, ensure you have the base development tools and OpenSSL installed:

```bash
# Arch
sudo pacman -S base-devel cmake openssl

# Ubuntu, Debian
sudo apt install base-devel cmake openssl

```

## Build Instructions

This project uses CMake for its build system. To compile:

```bash
# Generate the build system files
cmake -S . -B build

# Build the executable
cmake --build build
```

The compiled binary `mini-carver` will be located in the `build/` directory.

## Usage

```bash
mini-carver [OPTIONS] <target_image>
```

### Options

- -h, --help : Display the help manual and exit.
- -o : Specify the output directory where extracted files will be saved. Defaults
  to the current working directory (./) if omitted.

### Examples

Scan a raw disk image and extract files to the current directory:

```bash
./build/mini-carver foobar.raw
```

Scan a disk dump and save all recovered artifacts to a specific folder:

```bash
./build/mini-carver -o /home/user/foo bar.dd
```

Scan a block device:

```bash
# sudo is needed to access block device content on Linux
sudo ./build/mini-carver /dev/sda
```

## Technical Notes

The carver uses a finite state machine to manage extraction. Once a header is
matched, the engine transitions into an extraction state, piping bytes
simultaneously to the output file stream and the OpenSSL EVP_MD_CTX context.
This guarantees that the SHA-256 hash perfectly matches the recovered binary
exactly as it was carved from the raw data, preventing tampering and memory leaks.
