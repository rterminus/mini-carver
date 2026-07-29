/*!
 * @file main.cpp
 * @description A carver that outputs hashed identified files inside
 * binary files
 *
 * @author Renan Pontes (rterminus)
 * @date July 29th, 2026.
 */

#include <iostream>
#include "../include/carver.hpp"
#include "../include/cli.hpp"

//== Main entry

int main(int argc, char* argv[]) {
  RunningOpt opts = parse_arguments(argc, argv);
  Carver carver = Carver(opts);

  if (carver.open_image()) {
    carver.scan_image();
  } else {
    std::cerr << "[!] Error: Could not open target file or device.\n"
              << "    If you are trying to read a physical disk, (e.g., /dev/sda),\n"
                 "    make sure to run the program with root privileges.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
