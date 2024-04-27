//
// Created by jon on 4/13/24.
//

#include "abstract_syntax_tree.h"


struct NumberExprAST {
    double val;
};

struct CharExprAST {
    char val;
};

struct StringExprAST {
    std::string val;
};

struct LetExprAST {
    std::string name;
    std::unique_ptr<AST> val;
};

struct BinaryExprAST {
    std::string op;
    std::unique_ptr<AST> lhs, rhs;
};

struct CallExprAST {
    std::string callee;
    std::vector<std::unique_ptr<AST>> args;
};

struct PrototypeAST {
    std::string name;
    std::vector<std::string> args;
};

struct FunctionAST {
    std::unique_ptr<PrototypeAST> proto;
    std::vector<std::unique_ptr<AST>> body;
};

struct ReturnAST {
    std::unique_ptr<AST> val;
};

struct IfAST {
    std::unique_ptr<AST> cond;
    std::vector<std::unique_ptr<AST>> body_on_true, body_on_false;
};

struct UnitExprAST {

};

struct VariableExprAST {
    std::string name;
};

