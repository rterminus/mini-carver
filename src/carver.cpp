#include "../include/carver.hpp"

bool Carver::open_image() {
  m_stream.open(m_opts.image_path, std::ios::binary);

  return m_stream.is_open();
}

void Carver::scan_image() {}
