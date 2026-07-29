#include "../include/carver.hpp"
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

bool Carver::open_image() {
  m_stream.open(m_opts.image_path, std::ios::binary);

  return m_stream.is_open();
}

void Carver::scan_image() {
  std::ofstream out_file{};
  uint8_t file_count{ 0 };
  std::vector<uint8_t> stream_buffer(CHUNK_SIZE);
  std::vector<uint8_t> overlap_buffer(OVERLAP_SIZE, 0);
  bool extracting{ false };
  const FileSignature* active_sig{ nullptr };

  while (m_stream) {
    std::copy(overlap_buffer.begin(), overlap_buffer.end(), stream_buffer.begin());

    // converting first element of buffer to the required .read() type
    m_stream.read(reinterpret_cast<char*>(stream_buffer.data() + OVERLAP_SIZE),
                  CHUNK_SIZE - OVERLAP_SIZE);

    // exits loop if no bytes were read
    if (m_stream.gcount() == 0)
      break;

    size_t valid_bytes = OVERLAP_SIZE + m_stream.gcount();

    for (size_t idx{ 0 }; idx < valid_bytes; idx++) {
      // checks for headers to change extraction state
      if (!extracting) {
        // iterates in supported signatures to define file type
        for (const auto& sig : m_signatures) {
          if (match_signature(stream_buffer, sig.header, idx, valid_bytes)) {
            active_sig = &sig;

            // concatenates output file path + name
            std::string filename = m_opts.output_path.string() + "/file_"
                                   + std::to_string(file_count) + active_sig->extension;

            out_file.open(filename, std::ios::binary);
            extracting = true;
            file_count++;

            break;
          }
        }
      }

      // inserts continuously and tries to match footer signature while extracting
      // if matched, closes file
      if (extracting && out_file.is_open()) {
        out_file.put(stream_buffer[idx]);

        if (match_signature(stream_buffer, active_sig->footer, idx, valid_bytes)) {
          for (size_t footer_idx{ 1 }; footer_idx < active_sig->footer.size(); footer_idx++) {
            out_file.put(stream_buffer[idx + footer_idx]);
          }
          idx += active_sig->footer.size() - 1;

          out_file.close();
          extracting = false;
          active_sig = nullptr;
        }
      }
    }

    std::copy(stream_buffer.end() - OVERLAP_SIZE, stream_buffer.end(), overlap_buffer.begin());
  }
}

bool Carver::match_signature(const std::vector<uint8_t>& buffer,
                             const std::vector<uint8_t>& signatures,
                             size_t current_idx,
                             size_t valid_bytes) {
  // returns early if signature doesn't fit in remaining bytes
  if (current_idx + signatures.size() > valid_bytes) {
    return false;
  }

  for (size_t idx{ 0 }; idx < signatures.size(); idx++) {
    if (signatures[idx] != buffer[current_idx + idx])
      return false;
  }

  return true;
}
