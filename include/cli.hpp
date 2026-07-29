#ifndef CLI_HPP
#define CLI_HPP

#include <dirent.h>
#include <filesystem>
#include <string>
#include <vector>

using ushort = unsigned short;

/*!
 * @brief The running options provided via CLI.
 */
struct RunningOpt {
  std::vector<std::filesystem::path> input_list;
  ushort max_filename{8};
  bool is_recursive{false};
  bool is_ascending{false};
  bool is_descending{false};
  char table_order{'f'};
};

/*!
 * @brief Parses the command-line arguments.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return A RunningOpt struct containing the target files.
 */
RunningOpt parse_arguments(int argc, char* argv[]);

/*!
 * @brief Validates and assigns the sorting criteria based on the provided character.
 *
 * @param arg The sorting field character.
 * @param opts The running options to be updated.
 */
void validate_sorting_argument(char* arg, RunningOpt& opts);

/*!
 * @brief Validates a file or directory path and adds supported files to the input list.
 *
 * @param arg The file or directory path to validate.
 * @param opts The running options containing the input list and recursive flag.
 */
void validate_file_argument(const char* arg, RunningOpt& opts);

/*!
 * @brief Checks if a given file extension is supported by the counter.
 *
 * @param extension The file extension to check (e.g., ".cpp").
 * @return True if the extension is supported, false otherwise.
 */
bool is_supported_extension(const std::string& extension);

/*!
 * @brief Iterates through a directory and extracts all supported files.
 *
 * @tparam DirIterator The type of directory iterator (regular or recursive).
 * @param iterator The initialized directory iterator.
 * @param opts The running options to store the valid files and update max_filename.
 */
template <typename DirIterator>
void parse_supported_files(DirIterator iterator, RunningOpt& opts) {
  for (auto const& dir_entry : iterator) {
    std::string file_extension{dir_entry.path().extension().string()};

    if (is_supported_extension(file_extension)) {
      opts.input_list.emplace_back(dir_entry.path().string());
    }
  }
}

#endif
