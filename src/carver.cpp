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
  std::vector<uint8_t> stream_buffer(CHUNK_SIZE);
  std::vector<uint8_t> overlap_buffer(OVERLAP_SIZE, 0);

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
      if (!m_extracting) {
        check_for_header(stream_buffer, idx, valid_bytes);
      }

      // inserts continuously and tries to match footer signature while extracting
      // if matched, closes file
      if (m_extracting && m_out_file.is_open()) {
        m_out_file.put(stream_buffer[idx]);

        if (match_signature(stream_buffer, m_active_sig->footer, idx, valid_bytes)) {
          for (size_t footer_idx{ 1 }; footer_idx < m_active_sig->footer.size(); footer_idx++) {
            m_out_file.put(stream_buffer[idx + footer_idx]);
          }
          idx += m_active_sig->footer.size() - 1;

          m_out_file.close();
          m_extracting = false;
          m_active_sig = nullptr;
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

void Carver::check_for_header(const std::vector<uint8_t>& buffer,
                              size_t current_idx,
                              size_t valid_bytes) {
  // iterates in supported signatures to define file type
  for (const auto& sig : m_signatures) {
    if (match_signature(buffer, sig.header, current_idx, valid_bytes)) {
      m_active_sig = &sig;

      // concatenates output file path + name
      std::string filename = m_opts.output_path.string() + "/file_" + std::to_string(m_file_count)
                             + m_active_sig->extension;

      m_out_file.open(filename, std::ios::binary);
      m_extracting = true;
      m_file_count++;

      break;
    }
  }
}

void Carver::process_extraction(const std::vector<uint8_t>& buffer,
                                size_t& current_idx,
                                size_t valid_bytes) {

  m_out_file.put(buffer[current_idx]);

  if (match_signature(buffer, m_active_sig->footer, current_idx, valid_bytes)) {
    for (size_t footer_idx{ 1 }; footer_idx < m_active_sig->footer.size(); footer_idx++) {
      m_out_file.put(buffer[current_idx + footer_idx]);
    }
    current_idx += m_active_sig->footer.size() - 1;

    m_out_file.close();
    m_extracting = false;
    m_active_sig = nullptr;
  }
}
