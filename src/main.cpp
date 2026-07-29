/*!
 * @file main.cpp
 * @description
 * This program implements a single line of code count for C/C++ programs.
 * @author  Álvaro Barbosa & Renan Pontes
 * @date  June 3rd, 2026.
 */
#include <cstdlib>
#include <dirent.h>
#include <utility>
#include <vector>

#include "../include/fileinfo.hpp"
#include "../include/cli.hpp"
#include "../include/ui.hpp"

//== Main entry

int main(int argc, char* argv[]) {
    RunningOpt opts = parse_arguments(argc, argv);
    std::vector<FileInfo> processed_files;
    for (const auto& file_path : opts.input_list) {
        FileInfo current_file;
        if (current_file.open_file(file_path)) {
            processed_files.push_back(std::move(current_file));
        } else {
            std::cerr << "Sorry, unable to read " << file_path.string() << "\n";
        }
    }
    sort_fileinfo(processed_files, opts);
    output_table(processed_files, opts);

    return EXIT_SUCCESS;
}
