#ifndef CARVER_HPP
#define CARVER_HPP

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
  RunningOpt m_opts;                             //!< opts received from cli
  std::ifstream m_stream;                        //!< disk reading stream
  std::ofstream m_out_file;                      //!< output file
  std::string m_current_filename{ "" };          //!< current output file's name
  unsigned int m_file_count{ 0 };                //!< tracks outputed file count
  bool m_extracting{ false };                    //!< current state: searching/extracting
  const FileSignature* m_active_sig{ nullptr };  //!< active file signature if identified
  std::vector<FileSignature> m_signatures;       //!< supported file signatures

  static constexpr size_t CHUNK_SIZE = 4 * 1024 * 1024;  //!< sizeof 4MB
  static constexpr size_t OVERLAP_SIZE = 16;  //!< bytes that will be copied to next iteration

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

  /*!
   * @brief Checks for header sequences in binary stream.
   *
   * @param buffer Current working binary buffer.
   * @param current_idx Current working buffer index.
   * @param valid_bytes Number of valid working bytes.
   */
  void check_for_header(const std::vector<uint8_t>& buffer, size_t current_idx, size_t valid_bytes);

  /*!
   * @brief Processes extractions, writing valid bytes to buffer.
   *
   * @param buffer Current working binary buffer.
   * @param current_idx Current working buffer index.
   * @param valid_bytes Number of valid working bytes.
   */
  void process_extraction(const std::vector<uint8_t>& buffer,
                          size_t& current_idx,
                          size_t valid_bytes);
};

#endif  // CARVER_HPP
