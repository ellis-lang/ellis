//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_COMPILER_HPP
#define ELLIS_COMPILER_HPP

#include <vector>
#include <string>

class Compiler {
private:
    bool verbose;
public:
    Compiler(const bool verbose) : verbose(verbose) {}
    int compile(const std::vector<std::string>& files);
};


#endif //ELLIS_COMPILER_HPP
