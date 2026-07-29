#include "../include/carver.hpp"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <openssl/evp.h>
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
      m_current_filename = m_opts.output_path.string() + "/file_" + std::to_string(m_file_count)
                           + m_active_sig->extension;

      m_out_file.open(m_current_filename, std::ios::binary);

      std::cout << "[*] " << m_active_sig->extension << " file found on byte " << current_idx
                << "\n";

      m_extracting = true;
      m_file_count++;

      // preparing hash context for file extraction
      m_sha256_ctx = EVP_MD_CTX_new();
      EVP_DigestInit_ex(m_sha256_ctx, EVP_sha256(), nullptr);

      break;
    }
  }
}

void Carver::process_extraction(const std::vector<uint8_t>& buffer,
                                size_t& current_idx,
                                size_t valid_bytes) {

  m_out_file.put(buffer[current_idx]);
  // updates hash context for newly added file content
  EVP_DigestUpdate(m_sha256_ctx, &buffer[current_idx], 1);

  if (match_signature(buffer, m_active_sig->footer, current_idx, valid_bytes)) {
    for (size_t footer_idx{ 1 }; footer_idx < m_active_sig->footer.size(); footer_idx++) {
      m_out_file.put(buffer[current_idx + footer_idx]);
    }
    current_idx += m_active_sig->footer.size() - 1;

    m_out_file.close();

    // defines hash byte size
    unsigned char hash_bytes[EVP_MAX_MD_SIZE];
    unsigned int hash_len{ 0 };

    // closes hash context
    EVP_DigestFinal_ex(m_sha256_ctx, hash_bytes, &hash_len);
    // basically a destructor to avoid memory leaking
    // EVP_MD_CTX_new is the same as a new EVP_MD_CTX() for ex.
    EVP_MD_CTX_free(m_sha256_ctx);

    // preparing for hexadecimal conversion
    std::string hash_string{ "" };
    char hex_buffer[3];
    // converting hash in binary to a hexadecimal string
    // inserts converted bytes into a string stream with a buffer
    // for safe insertion (up to 3 hex chars)
    for (unsigned int idx{ 0 }; idx < hash_len; idx++) {
      std::sprintf(hex_buffer, "%02x", hash_bytes[idx]);
      hash_string += hex_buffer;
    }

    std::cout << "[+] File recovered and saved as " << m_current_filename << ".\n";
    std::cout << " | SHA-256: " << hash_string << "\n";

    m_extracting = false;
    m_active_sig = nullptr;
  }
}
