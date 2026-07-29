#include "../include/ui.hpp"
#include <iostream>
#include "../lib/tcolor.h"

void print_help() {
  std::cout << "Welcome to Mini Carver, version 1.0.\n\n"
            << TColor::colorize("NAME\n", TColor::BOLD)
            << "    mini-carver - a lightweight binary file carver for forensic recovery.\n\n"
            << TColor::colorize("SYNOPSIS\n", TColor::BOLD)
            << "    mini-carver [OPTIONS] <target_image>\n\n"
            << TColor::colorize("DESCRIPTION\n", TColor::BOLD)
            << "    Mini Carver scans raw disk images or binary dumps byte-by-byte\n"
            << "    using a sliding window algorithm, identifying file headers and\n"
            << "    footers to extract lost files without relying on the file system.\n\n"
            << TColor::colorize("SUPPORTED FORMATS\n", TColor::BOLD)
            << "    Disk Images: .raw, .img, .dd, .bin, .iso\n"
            << "    Extracted Files: .jpg (JPEG), .pdf (PDF)\n\n"
            << TColor::colorize("OPTIONS\n", TColor::BOLD) << "    -h, --help\n"
            << "        Display this help manual and exit.\n\n"
            << "    -o <directory>\n"
            << "        Specify the output directory where extracted files will be saved.\n"
            << "        (Defaults to the current working directory '.' if omitted).\n\n"
            << TColor::colorize("EXAMPLES\n", TColor::BOLD)
            << "    mini-carver disk.raw\n"
            << "        Scans the raw image and extracts files to the current directory.\n\n"
            << "    mini-carver -o /foo/bar/ disk.img\n"
            << "        Scans the disk image and saves all recovered artifacts into the\n"
            << "        specified evidence directory.\n";

  std::exit(0);
}

void print_error(ErrorType error, std::string optional) {
  if (error == ErrorType::MISSING_ARGUMENT) {
    std::cout << "Sorry, the argument for \"" << optional << "\" is missing at this time.";
  } else if (error == ErrorType::PATH_NOT_FOUND) {
    std::cout << "Sorry, the path for \"" << optional << "\" was not found at this time.";
  } else if (error == ErrorType::UNSUPPORTED_EXTENSION) {
    std::cout << "Sorry, \"" << optional << "\" files are not supported at this time.";
  } else if (error == ErrorType::NO_SUPPORTED_FILE) {
    std::cout << "Sorry, there was no supported file provided at this time.";
  } else {
    std::cout << "Sorry, unable to run the program right now.";
  }

  std::exit(0);
}
