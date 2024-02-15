project(EllisLib)


add_library(ellis STATIC compiler.hpp compiler.cpp
        lex.cpp
        lex.hpp
        source_handler.hpp
        ast.cpp
        ast.hpp
        parser.cpp
        parser.hpp)