//
// Created by jonathan on 12/14/23.
//

#ifndef ELLIS_SOURCE_HANDLER_HPP
#define ELLIS_SOURCE_HANDLER_HPP

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

std::string read_file(std::string filename) {
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

#endif //ELLIS_SOURCE_HANDLER_HPP
