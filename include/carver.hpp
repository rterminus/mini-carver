#ifndef CARVER_HPP
#define CARVER_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>
#include "./cli.hpp"

struct FileSignature {
  std::string extension{};
  std::vector<uint8_t> header{};
  std::vector<uint8_t> footer{};
};

class Carver {
private:
  RunningOpt m_opts;                        //!< opts received from cli
  std::ifstream m_stream;                   //!< disk reading stream
  std::vector<FileSignature> m_signatures;  //!< supported file signatures

  static constexpr size_t CHUNK_SIZE = 4 * 1024 * 1024;  //!< 4MB
  static constexpr size_t OVERLAP_SIZE = 16;  //!< bytes that will be copied to next iteration
                                              // (sliding window)
  static constexpr std::array<uint8_t, 2> JPEG_FOOTER = { 0xFF, 0xD9 };

public:
  Carver(RunningOpt opts) : m_opts(opts) {
    // JPEG signature
    m_signatures.emplace_back(FileSignature{ ".jpg", { 0xFF, 0xD8, 0xFF }, { 0xFF, 0xD9 } });

    // PDF signature
    m_signatures.emplace_back(
      FileSignature{ ".pdf", { 0x25, 0x50, 0x44, 0x46 }, { 0x25, 0x45, 0x4F, 0x46 } });
  };  //!< default class ctro. receiving cli opts and initializing
      // supported signatures

  /*!
   * @brief Opens image stream safely for raw reading.
   * @return True if successfully opened, false otherwise.
   */
  bool open_image();

  /*!
   * @brief Scans disk, finds signatures and extracts files.
   */
  void scan_image();

  /*!
   * @brief Matches a sequence of bytes to supported signatures.
   *
   * @param buffer Current working binary buffer.
   * @param signature File specific signatures.
   * @param current_idx Current working buffer index.
   * @param valid_bytes Number of valid working bytes.
   * @return True if successfully matched, false otherwise.
   */
  bool match_signature(const std::vector<uint8_t>& buffer,
                       const std::vector<uint8_t>& signatures,
                       size_t current_idx,
                       size_t valid_bytes);
};

#endif  // CARVER_HPP
