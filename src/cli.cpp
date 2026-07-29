#include "../include/cli.hpp"
#include <filesystem>
#include "../include/ui.hpp"

RunningOpt parse_arguments(int argc, char* argv[]) {
  RunningOpt opts{};
  std::filesystem::path target_image{ "" };
  std::filesystem::path target_output{ "" };

  if (argc == 1) {
    print_help();
  }

  for (unsigned short i{ 1 }; i < argc; ++i) {
    if (argv[i][0] == '-') {
      std::string arg = argv[i];

      if (arg == "-h" || arg == "--help") {
        print_help();
      } else if (arg == "-o") {
        if (i + 1 < argc) {
          target_output = argv[i + 1];
          i++;
        } else {
          print_error(ErrorType::MISSING_ARGUMENT, arg);
        }
      }
    } else {
      // Non-flag arguments are treated as the target file
      target_image = argv[i];
    }
  }

  // Ensure at least one valid file was captured to process
  if (target_image == "") {
    print_error(ErrorType::NO_SUPPORTED_FILE);
  }

  validate_arguments(target_image, target_output, opts);

  return opts;
}

void validate_arguments(std::filesystem::path target_image,
                        std::filesystem::path target_output,
                        RunningOpt& opts) {
  if (!std::filesystem::exists(target_image)) {
    print_error(ErrorType::PATH_NOT_FOUND, target_image);
  }

  if (target_output.empty()) {
    opts.output_path = ".";
  } else {
    if (std::filesystem::exists(target_output) and std::filesystem::is_directory(target_output)) {
      opts.output_path = target_output;
    } else {
      print_error(ErrorType::PATH_NOT_FOUND, target_output);
    }
  }

  if (target_image != "" and std::filesystem::is_regular_file(target_image)) {
    std::string file_extension{ std::filesystem::path(target_image).extension() };

    if (is_supported_extension(file_extension)) {
      opts.image_path = target_image;
    } else {
      print_error(ErrorType::UNSUPPORTED_EXTENSION, file_extension);
    }
  }
}

bool is_supported_extension(const std::string& extension) {
  return (extension == ".raw" || extension == ".img" || extension == ".dd" || extension == ".bin"
          || extension == ".iso");
}
