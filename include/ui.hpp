#ifndef UI_HPP
#define UI_HPP

#include "./cli.hpp"
#include "./fileinfo.hpp"
#include <string>

/*!
 * @brief Defines the types of errors that can occur during execution.
 */
enum class ErrorType {
  INVALID_OPTION = 0,
  INVALID_SORT_FIELD,
  MISSING_ARGUMENT,
  PATH_NOT_FOUND,
  UNSUPPORTED_EXTENSION,
  NO_SUPPORTED_FILE
};

/*!
 * @brief Prints the help manual and exits the program.
 */
void print_help();

/*!
 * @brief Prints a specific error message and exits the program.
 *
 * @param error The type of error encountered.
 * @param optional Additional context for the error.
 */
void print_error(ErrorType error, std::string optional = "");

/*!
 * @brief Formats and prints the summary table.
 *
 * @param file_info Constant reference to the processed files data.
 * @param opts The parsed command-line options containing formatting preferences.
 */
void output_table(const std::vector<FileInfo>& file_info, RunningOpt& opts);

/*!
 * @brief Converts the language enumeration into string format.
 *
 * @param lang The lang_type_e enum value.
 * @return A string representing the language.
 */
std::string lang_to_string(lang_type_e lang);

/*!
 * @brief Formats a metric and its percentage relative to the total into a single string.
 *
 * @param value The specific line count.
 * @param total The total number of lines in the file.
 * @return A formatted string in the format "value (percent%)".
 */
std::string format_with_percent(count_t value, count_t total);

#endif // UI_HPP
