cmake_minimum_required(VERSION 3.27)
project(Ellis)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(ellisc main.cpp include/argparse.hpp src/compiler.hpp src/compiler.cpp
        src/lex.cpp
        src/lex.hpp
        src/source_handler.hpp)
