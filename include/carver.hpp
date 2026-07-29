#ifndef CARVER_HPP
#define CARVER_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include "./cli.hpp"

class Carver {
private:
  RunningOpt m_opts;       //!< opts received from cli
  std::ifstream m_stream;  //!< disk reading stream

  static constexpr size_t CHUNK_SIZE = 4 * 1024 * 1024;  //!< 4MB
  static constexpr size_t OVERLAP_SIZE = 16;  //!< bytes that will be copied to next iteration
                                              // (sliding window)

  static constexpr std::array<uint8_t, 3> JPEG_HEADER = { 0xFF, 0xD8, 0xFF };
  static constexpr std::array<uint8_t, 2> JPEG_FOOTER = { 0xFF, 0xD9 };

public:
  Carver(RunningOpt opts) : m_opts(opts) {};  //!< default class ctro. receiving cli opts

  /*!
   * @brief Opens image stream safely for raw reading.
   * @return True if successfully opened, false otherwise.
   */
  bool open_image();

  /*!
   * @brief Scans disk, finds signatures and extracts files.
   */
  void scan_image();
};

#endif  // CARVER_HPP
