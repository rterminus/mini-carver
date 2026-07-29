#include "../include/fileinfo.hpp"

lang_type_e FileInfo::which_lang(std::string& name_file) {
  std::string format = std::filesystem::path(name_file).extension().string();
  if (format == ".cpp") {
    return CPP;
  }
  if (format == ".hpp") {
    return HPP;
  }
  if (format == ".c") {
    return C;
  }
  if (format == ".h") {
    return H;
  }
  return UNDEF;
}

void FileInfo::parse_file(std::ifstream& stream) {
  std::string line;
  FSM state_machine;

  while (grab_line(stream, line)) {
    parse_line(line, state_machine);
  }
}

void FileInfo::parse_line(const std::string& line, FSM& state_machine) {
  std::string trimmed{ trim(line) };

  // Blank line treatment.
  if (trimmed.empty()) {
    if (state_machine.get_state() == CODE) {
      n_blank++;
      return;
    }
    if (state_machine.get_state() == BLOCK_COMMENT) {
      n_comments++;
      return;
    }
    if (state_machine.get_state() == BLOCK_DOXYGEN) {
      n_doxycomments++;
      return;
    }
  }

  bool has_code{ false };
  bool has_comment{ false };
  bool has_doxygen{ false };

  for (std::size_t idx{ 0 }; idx < trimmed.size(); idx++) {
    switch (state_machine.get_state()) {
    case BLOCK_COMMENT:
    case BLOCK_DOXYGEN:
      has_comment = true;
      if (state_machine.get_state() == BLOCK_DOXYGEN)
        has_doxygen = true;

      // Checks line limits to avoid seg fault
      if (idx + 1 < trimmed.size() && trimmed[idx] == '*' && trimmed[idx + 1] == '/') {
        state_machine.set_state(CODE);
        idx++; 
      }
      break;

    case IN_STRING:
      has_code = true;
      if (trimmed[idx] == '\\') {
        idx++;
      } else if (trimmed[idx] == '"') {
        state_machine.set_state(CODE);
      }
      break;

    case RAW_STRING:
      has_code = true;
      // Seg fault checking
      if (idx + 1 < trimmed.size() && trimmed[idx] == ')' && trimmed[idx + 1] == '"') {
        state_machine.set_state(CODE);
        idx++;
      }
      break;

    case CODE:
      if (trimmed[idx] == '/') {
        // Line comment treatment
        if (idx + 1 < trimmed.size() && trimmed[idx + 1] == '/') {
          has_comment = true;
          if (idx + 2 < trimmed.size() && (trimmed[idx + 2] == '/' || trimmed[idx + 2] == '!')) {
            has_doxygen = true;
          }
          // Sets idx as line size to skip it
          idx = trimmed.size();
          break;
        }
        // Block comment treatment
        if (idx + 1 < trimmed.size() && trimmed[idx + 1] == '*') {
          has_comment = true;
          if (idx + 2 < trimmed.size() && (trimmed[idx + 2] == '*' || trimmed[idx + 2] == '!')) {
            state_machine.set_state(BLOCK_DOXYGEN);
            has_doxygen = true;
          } else {
            state_machine.set_state(BLOCK_COMMENT);
          }
          idx++;
          break;
        }
      }
      // Raw string treatment
      else if (idx + 2 < trimmed.size() && trimmed[idx] == 'R' && trimmed[idx + 1] == '"' && trimmed[idx + 2] == '(') {
        has_code = true;
        state_machine.set_state(RAW_STRING);
        idx += 2;
      }
      // Normal string treatment
      else if (trimmed[idx] == '"') {
        has_code = true;
        state_machine.set_state(IN_STRING);
      }
      // Pure code treatment
      else if (trimmed[idx] != ' ' && trimmed[idx] != '\t') {
        has_code = true;
      }
      break;
    }
  }

  // Final count addition
  if (has_code) {
    n_loc++;
  }
  
  if (has_doxygen) {
    n_doxycomments++;
  } else if (has_comment) {
    n_comments++;
  }
}

bool FileInfo::open_file(const std::filesystem::path& file_path) {
  this->filename = file_path.string();
  this->type = which_lang(this->filename);

  std::ifstream stream(file_path);
  if (stream.is_open()) {
    parse_file(stream);
    stream.close();
    return true;
  }
  return false;
}

bool FileInfo::grab_line(std::ifstream& stream, std::string& this_line) {
  if (std::getline(stream, this_line)) {
    n_lines++;
    return true;
  }
  return false;
}
