/*!
 * @file main.cpp
 * @description A carver that outputs hashed identified files inside
 * binary files
 *
 * @author Renan Pontes (rterminus)
 * @date July 29th, 2026.
 */

#include "../include/carver.hpp"
#include "../include/cli.hpp"

//== Main entry

int main(int argc, char* argv[]) {
  RunningOpt opts = parse_arguments(argc, argv);
  Carver carver = Carver(opts);

  if (carver.open_image()) {
    carver.scan_image();
  }

  return EXIT_SUCCESS;
}
