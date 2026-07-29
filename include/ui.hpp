#ifndef UI_HPP
#define UI_HPP

#include <string>

/*!
 * @brief Defines the types of errors that can occur during execution.
 */
enum class ErrorType {
  MISSING_ARGUMENT = 0,
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

#endif // UI_HPP
