#include "../include/carver.hpp"
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
