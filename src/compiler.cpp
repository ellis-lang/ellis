//
// Created by jonathan on 12/14/23.
//

#include "compiler.hpp"
#include "source_handler.hpp"
#include "lex.hpp"


int Compiler::compile(const std::vector<std::string>& files) {
    for (auto file : files) {
        auto file_string = read_file(file);
        if (verbose) {
            std::cout << "File: " << file << "\n";
            std::cout << "contents: \n";
            std::cout << file_string;
            std::cout << "\n";
        }
        auto tokens = lex(file_string, verbose);

    }
    return 0;
}
