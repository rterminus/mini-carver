#ifndef CLI_HPP
#define CLI_HPP

#include <filesystem>
#include <string>

using ushort = unsigned short;

/*!
 * @brief The running options provided via CLI.
 */
struct RunningOpt {
  std::filesystem::path image_path{ "" };
  std::filesystem::path output_path{ "" };
};

/*!
 * @brief Parses the command-line arguments.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return A RunningOpt struct containing the target file and optional output directory.
 */
RunningOpt parse_arguments(int argc, char* argv[]);

/*!
 * @brief Validates a file and optional directory and inserts into opts if valid.
 *
 * @param target_image The file to validate.
 * @param target_output The directory to validate.
 * @param opts The running options containing the target file and optional output directory.
 */
void validate_arguments(std::filesystem::path target_image,
                        std::filesystem::path target_output,
                        RunningOpt& opts);

/*!
 * @brief Checks if a given file extension is supported by the carver.
 *
 * @param extension The file extension to check (e.g., ".bin").
 * @return True if the extension is supported, false otherwise.
 */
bool is_supported_extension(const std::string& extension);

#endif
