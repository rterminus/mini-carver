#ifndef FILEINFO_HPP
#define FILEINFO_HPP

#include <cstdint>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <linux/limits.h>
#include <string>
#include <utility>

#include "./fsm.hpp"

/*!
 * @brief Defines supported languages.
 */
enum lang_type_e : std::uint8_t {
  C = 0,  //!< C language
  CPP,    //!< C++ language
  H,      //!< C/C++ header
  HPP,    //!< C++ header
  UNDEF,  //!< Undefined type.
};

//== Class/Struct declaration

/// Integer type for counting lines.
using count_t = unsigned long;
/*!
 * @brief Encapsulates storage and processing of file information.
 */
class FileInfo {
  std::string filename;         //!< the filename.
  lang_type_e type{ CPP };      //!< the language type.
  count_t n_blank{ 0 };         //!< # of blank lines in the file.
  count_t n_comments{ 0 };      //!< # of comment lines.
  count_t n_doxycomments{ 0 };  //!< # of Doxygen comments.
  count_t n_loc{ 0 };           //!< # lines of code.
  count_t n_lines{ 0 };         //!< # of lines.

public:
  /*!
   * @brief Class constructor. Accepts optional parameters.
   *
   * @param fn Filename. Defaults to empty string.
   * @param t Filetype. Defaults to UNDEF.
   * @param nb Blank line count. Defaults to 0.
   * @param nc Comment line count. Defaults to 0.
   * @param nd Doxygen comment line count. Defaults to 0.
   * @param nl Lines of code line count. Defaults to 0.
   * @param ni Total line count. Defaults to 0.
   */
  FileInfo(std::string fn = "",
           lang_type_e t = UNDEF,
           count_t nb = 0,
           count_t nc = 0,
           count_t nd = 0,
           count_t nl = 0,
           count_t ni = 0)
      : filename{ std::move(fn) }, type{ t }, n_blank{ nb }, n_comments{ nc }, n_doxycomments{ nd },
        n_loc{ nl }, n_lines{ ni } {}

  /*!
   * @brief Identifies current file language.
   *
   * @param name_file Name of file.
   * @return lang_type_e enum equivalent of filetype.
   */
  lang_type_e which_lang(std::string& name_file);
  /*!
   * @brief Executes file parsing loop.
   * 
   * @param stream Current file stream.
   */
  void parse_file(std::ifstream& stream);
  /*!
   * @brief Parses current file.
   *
   * @param line Current line in form of string.
   * @param state_machine Current state of the machine loop.
   */
  void parse_line(const std::string& line, FSM& state_machine);
  /*!
   * @brief Opens, processes and closes inputted file stream.
   *
   * @param file_path Path of the file to be processed.
   * @return True if file was opened; false otherwise.
   */
  bool open_file(const std::filesystem::path& file_path);
  /*!
   * @brief Grabs line and increments total line count.
   *
   * @param stream File stream to grab line from.
   * @param this_line Current line in form of string.
   */
  bool grab_line(std::ifstream& stream, std::string& this_line);

  /*!
   * @brief Gets private member 'filename'.
   */
  std::string get_filename() const { return filename; }
  /*!
   * @brief Gets private member 'type' (filetype).
   */
  lang_type_e get_filetype() const { return type; }
  /*!
   * @brief Gets private member 'n_lines' (total line count).
   */
  count_t get_total_lines() const { return n_lines; }
  /*!
   * @brief Gets private member 'n_blank' (blank line count).
   */
  count_t get_blank_lines() const { return n_blank; }
  /*!
   * @brief Gets private member 'n_doxycomments' (doxygen comment line count).
   */
  count_t get_doxy_lines() const { return n_doxycomments; }
  /*!
   * @brief Gets private member 'n_comments' (comment line count).
   */
  count_t get_comment_lines() const { return n_comments; }
  /*!
   * @brief Gets private member 'n_loc' (lines of code count).
   */
  count_t get_code_lines() const { return n_loc; }
};

#endif
