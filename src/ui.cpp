#include "../lib/tcolor.h"
#include "../include/ui.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

void print_help() {
  std::cout << "Welcome to Mini Carver, version 1.0.\n\n"
            << TColor::colorize("NAME\n", TColor::BOLD)
            << "  mini carver - mini binary file carver.\n\n"
            << TColor::colorize("SYNOPSIS\n", TColor::BOLD)
            << "  mini-carver <file>\n\n"
            << TColor::colorize("EXAMPLES\n", TColor::BOLD) << R"(  sloc main.cpp sloc.cpp
     Counts loc, comments, blanks of the source files 'main.cpp' and 'sloc.cpp'

  sloc source
     Counts loc, comments, blanks of all C/C++ source files inside 'source'

  sloc -r -s c source
     Counts loc, comments, blanks of all C/C++ source files recursively inside 'source'
     and sort the result in ascending order by the number of comment lines.
            )"
            << "\n\n"
            << TColor::colorize("DESCRIPTION\n", TColor::BOLD)
            << R"(  Sloc counts the individual number **lines of code** (LOC), comments, and blank
  lines found in a list of files or directories passed as the last argument
  (after options).
  After the counting process is concluded the program prints out to the standard
  output a table summarizing the information gathered, by each source file and/or
  directory provided.
  It is possible to inform which fields sloc should use to sort the data by, as
  well as if the data should be presented in ascending/descending numeric order.
            )"
            << "\n\n"
            << TColor::colorize("OPTIONS:\n", TColor::BOLD) << R"(  -h/--help
            Display this information.

  -r
            Look for files recursively in the directory provided.

  -s f|t|c|d|b|s|a
            Sort table in ASCENDING order by (f)ilename, (t) filetype,
            (c)omments, (d)oc comments, (b)lank lines, (s)loc, or (a)ll.
            Default is to show files in ordem of appearance.

  -S f|t|c|d|b|s|a
            Sort table in DESCENDING order by (f)ilename, (t) filetype,
            (c)omments, (d)oc comments, (b)lank lines, (s)loc, or (a)ll.
            Default is to show files in order of appearance.
  )";
  std::exit(0);
}

void print_error(ErrorType error, std::string optional) {
  if (error == ErrorType::INVALID_OPTION) {
    std::cout << "Sorry, \"" << optional << "\" is not a valid option at this time.";
  } else if (error == ErrorType::INVALID_SORT_FIELD) {
    std::cout << "Sorry, \"" << optional << "\" is not a valid sorting option at this time.";
  } else if (error == ErrorType::MISSING_ARGUMENT) {
    std::cout << "Sorry, the argument for \"" << optional << "\" is missing at this time.";
  } else if (error == ErrorType::PATH_NOT_FOUND) {
    std::cout << "Sorry, the path for \"" << optional << "\" was not found at this time.";
  } else if (error == ErrorType::UNSUPPORTED_EXTENSION) {
    std::cout << "Sorry, \"" << optional << "\" files are not supported at this time.";
  } else if (error == ErrorType::NO_SUPPORTED_FILES) {
    std::cout << "Sorry, there was no supported files provided at this time.";
  } else {
    std::cout << "Sorry, unable to run the program right now.";
  }

  std::exit(0);
}

void sort_fileinfo(std::vector<FileInfo>& file_info, RunningOpt& opts) {
  // Early return to preserve the original reading order if no sorting flag was provided
  if (!opts.is_ascending && !opts.is_descending) {
    return;
  }

  std::sort(file_info.begin(), file_info.end(), [&opts](const FileInfo& lhs, const FileInfo& rhs) {
    // Ternary operators are used to invert the comparison dynamically for descending order
    if (opts.table_order == 'f') {
      return opts.is_descending ? (rhs.get_filename() < lhs.get_filename())
                                : (lhs.get_filename() < rhs.get_filename());
    } else if (opts.table_order == 't') {
      std::string lhs_extension{ std::filesystem::path(lhs.get_filename()).extension().string() };
      std::string rhs_extension{ std::filesystem::path(rhs.get_filename()).extension().string() };
      return opts.is_descending ? (rhs_extension < lhs_extension) : (lhs_extension < rhs_extension);
    } else if (opts.table_order == 'c') {
      return opts.is_descending ? (rhs.get_comment_lines() < lhs.get_comment_lines())
                                : (lhs.get_comment_lines() < rhs.get_comment_lines());
    } else if (opts.table_order == 'd') {
      return opts.is_descending ? (rhs.get_doxy_lines() < lhs.get_doxy_lines())
                                : (lhs.get_doxy_lines() < rhs.get_doxy_lines());
    } else if (opts.table_order == 'b') {
      return opts.is_descending ? (rhs.get_blank_lines() < lhs.get_blank_lines())
                                : (lhs.get_blank_lines() < rhs.get_blank_lines());
    } else if (opts.table_order == 's') {
      return opts.is_descending ? (rhs.get_code_lines() < lhs.get_code_lines())
                                : (lhs.get_code_lines() < rhs.get_code_lines());
    } else {
      return opts.is_descending ? (rhs.get_total_lines() < lhs.get_total_lines())
                                : (lhs.get_total_lines() < rhs.get_total_lines());
    }
  });
}

void output_table(const std::vector<FileInfo>& file_info, RunningOpt& opts) {
  constexpr unsigned short TABLE_TABULATION{8};
  constexpr unsigned short COLUMN_WIDTH{16};
  constexpr unsigned short NUM_COLUMNS{6};

  std::cout << "Files processed: " << file_info.size() << "\n";

  // Dynamic table width to accommodate the longest parsed filename
  int TABLE_WIDTH{ opts.max_filename + TABLE_TABULATION + COLUMN_WIDTH * NUM_COLUMNS };

  std::cout << std::setfill('-') << std::setw(TABLE_WIDTH) << "" << "\n";
  std::cout << std::setfill(' ');
  std::cout << std::left
            << std::setw(opts.max_filename + TABLE_TABULATION) << "Filename"
            << std::setw(COLUMN_WIDTH) << "Language"
            << std::setw(COLUMN_WIDTH) << "Comments"
            << std::setw(COLUMN_WIDTH) << "Dox Comments"
            << std::setw(COLUMN_WIDTH) << "Blank"
            << std::setw(COLUMN_WIDTH) << "Code"
            << std::setw(COLUMN_WIDTH) << "# of lines"
            << "\n";
  std::cout << std::setfill('-') << std::setw(TABLE_WIDTH) << "" << "\n";
  std::cout << std::setfill(' ');

  int sum_comments{0};
  int sum_doxygen{0};
  int sum_blank{0};
  int sum_code{0};
  int sum_total{0};

  for (const auto& file : file_info) {
    int total = file.get_total_lines();

    sum_comments += file.get_comment_lines();
    sum_doxygen += file.get_doxy_lines();
    sum_blank += file.get_blank_lines();
    sum_code += file.get_code_lines();
    sum_total += total;

    std::cout << std::left
              << std::setw(opts.max_filename + TABLE_TABULATION) << file.get_filename()
              << std::setw(COLUMN_WIDTH) << lang_to_string(file.get_filetype())
              << std::setw(COLUMN_WIDTH) << format_with_percent(file.get_comment_lines(), total)
              << std::setw(COLUMN_WIDTH) << format_with_percent(file.get_doxy_lines(), total)
              << std::setw(COLUMN_WIDTH) << format_with_percent(file.get_blank_lines(), total)
              << std::setw(COLUMN_WIDTH) << format_with_percent(file.get_code_lines(), total)
              << std::setw(COLUMN_WIDTH) << total
              << "\n";
  }

  if (file_info.size() > 1) {
    std::cout << std::setfill('-') << std::setw(TABLE_WIDTH) << "" << "\n";
    std::cout << std::setfill(' ');
    std::cout << std::setw(opts.max_filename + TABLE_TABULATION) << "SUM"
      << std::setw(COLUMN_WIDTH) << " "
      << std::setw(COLUMN_WIDTH) << sum_comments
      << std::setw(COLUMN_WIDTH) << sum_doxygen
      << std::setw(COLUMN_WIDTH) << sum_blank
      << std::setw(COLUMN_WIDTH) << sum_code
      << std::setw(COLUMN_WIDTH) << sum_total
      << "\n";
  }
  std::cout << std::setfill('-') << std::setw(TABLE_WIDTH) << "" << "\n";
  std::cout << std::setfill(' ');
}

std::string lang_to_string(lang_type_e lang) {
  switch (lang) {
    case C:
      return "C";
    case CPP:
      return "C++";
    case H:
      return "C/C++ header";
    case HPP:
      return "C++ header";
    default:
      return "Undefined";
  }
}

std::string format_with_percent(count_t value, count_t total) {
  std::stringstream ss;

  // Prevent division by zero on completely empty files
  if (total == 0) {
    ss << value << " (0.0%)";
    return ss.str();
  }

  double percentage{(static_cast<double>(value) * 100) / total};

  ss << value << " (" << std::fixed << std::setprecision(1) << percentage << "%)";

  return ss.str();
}
