#include "../include/cli.hpp"
#include "../include/ui.hpp"

RunningOpt parse_arguments(int argc, char* argv[]) {
  RunningOpt opts{};
  std::vector<char*> target_paths{};

  if (argc == 1) {
    print_help();
  }

  for (unsigned short i{ 1 }; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = argv[i];

      if (arg == "-h" || arg == "--help") {
        print_help();
      } else if (arg == "-r") {
        opts.is_recursive = true;
      } else if (arg == "-s") {
        opts.is_ascending = true;

        // Ensure there is an argument in succession containing the sorting field
        if (i + 1 < argc) {
          validate_sorting_argument(argv[i+1], opts);
          i++;
        } else {
          print_error(ErrorType::MISSING_ARGUMENT, argv[i]);
        }
      } else if (arg == "-S") {
        opts.is_descending = true;

        if (i + 1 < argc) {
          validate_sorting_argument(argv[i+1], opts);
          i++;
        } else {
          print_error(ErrorType::MISSING_ARGUMENT, argv[i]);
        }
      } else {
        print_error(ErrorType::INVALID_OPTION, argv[i]);
      }
    } else {
      // Non-flag arguments are treated as target files or directories
      target_paths.push_back(argv[i]);
    }
  }

  for (const char* path : target_paths) {
    validate_file_argument(path, opts);
  }

  // Ensure at least one valid file was captured to process
  if (opts.input_list.empty()) {
    print_error(ErrorType::NO_SUPPORTED_FILES);
  }

  for (const auto& file_path : opts.input_list) {
      // Checks and updates max_filename based on filename length
      ushort current_length = std::string(file_path).length();
      if (current_length > opts.max_filename) {
        opts.max_filename = current_length;
      }
  }


  return opts;
}

void validate_sorting_argument(char* arg, RunningOpt& opts) {
  if (*arg == 'f') {
    opts.table_order = 'f';
  } else if (*arg == 't') {
    opts.table_order = 't';
  } else if (*arg == 'c') {
    opts.table_order = 'c';
  } else if (*arg == 'd') {
    opts.table_order = 'd';
  } else if (*arg == 'b') {
    opts.table_order = 'b';
  } else if (*arg == 's') {
    opts.table_order = 's';
  } else if (*arg == 'a') {
    opts.table_order = 'a';
  } else {
    print_error(ErrorType::INVALID_SORT_FIELD, arg);
  }
}

void validate_file_argument(const char* arg, RunningOpt& opts) {
  if (!std::filesystem::exists(arg)) {
    print_error(ErrorType::PATH_NOT_FOUND, arg);
  }

  if (std::filesystem::is_directory(arg)) {
    // Adapts to selected iterator type based on flag
    if (opts.is_recursive) {
      parse_supported_files(std::filesystem::recursive_directory_iterator{arg}, opts);
    } else {
      parse_supported_files(std::filesystem::directory_iterator{arg}, opts);
    }
  } else {
    std::string file_extension{std::filesystem::path(arg).extension()};

    if (is_supported_extension(file_extension)) {
      opts.input_list.emplace_back(arg);
    } else {
      print_error(ErrorType::UNSUPPORTED_EXTENSION, file_extension);
    }
  }
}

bool is_supported_extension(const std::string& extension) {
    return (extension == ".cpp" || extension == ".c"
            || extension == ".hpp" || extension == ".h");
}
