//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_AST_HPP
#define ELLIS_AST_HPP

#include <iostream>
#include <utility>
#include <variant>
#include <memory>
#include <vector>


struct NumberExprAST;
struct CharExprAST;
struct StringExprAST;
struct UnitExprAST;
struct VariableExprAST;
struct LetExprAST;
struct BinaryExprAST;
struct FunctionAST;
struct CallExprAST;
struct PrototypeAST;
struct ReturnAST;
struct IfAST;


using AST = std::variant<NumberExprAST, CallExprAST, CharExprAST, StringExprAST,
        UnitExprAST, VariableExprAST, LetExprAST, BinaryExprAST, FunctionAST, PrototypeAST,
        ReturnAST, IfAST>;


struct NumberExprAST {
    double val;
    NumberExprAST(double v): val(v) {}
};

struct CharExprAST {
    char val;
    CharExprAST(char v): val(v) {}
};

struct StringExprAST {
    std::string val;
    StringExprAST(std::string v): val(std::move(v)) {}
};

struct LetExprAST {
    std::string name;
    std::unique_ptr<AST> val;
    LetExprAST(std::string n, std::unique_ptr<AST> v): name(std::move(n)), val(std::move(v)) {}
};

struct BinaryExprAST {
    std::string op;
    std::unique_ptr<AST> lhs, rhs;
    BinaryExprAST(std::string op, std::unique_ptr<AST> l, std::unique_ptr<AST> r): op(op), lhs(std::move(l)), rhs(std::move(r)) {}
};

struct CallExprAST {
    std::string callee;
    std::vector<std::unique_ptr<AST>> args;
    CallExprAST(std::string c, std::vector<std::unique_ptr<AST>> a): callee(std::move(c)), args(std::move(a)) {}
};

struct PrototypeAST {
    std::string name;
    std::vector<std::string> args;
    PrototypeAST(std::string n, std::vector<std::string> a): name(std::move(n)), args(std::move(a)) {}
};

struct FunctionAST {
    std::unique_ptr<PrototypeAST> proto;
    std::vector<std::unique_ptr<AST>> body;
    FunctionAST(std::unique_ptr<PrototypeAST> p, std::vector<std::unique_ptr<AST>> b): proto(std::move(p)), body(std::move(b)) {}
};

struct ReturnAST {
    std::unique_ptr<AST> val;
    ReturnAST(std::unique_ptr<AST> v): val(std::move(v)) {}
    ReturnAST() {}
};

struct IfAST {
    std::unique_ptr<AST> cond;
    std::vector<std::unique_ptr<AST>> body_on_true, body_on_false;
    IfAST(std::unique_ptr<AST> c, std::vector<std::unique_ptr<AST>> t, std::vector<std::unique_ptr<AST>> f):
        cond(std::move(c)), body_on_true(std::move(t)), body_on_false(std::move(f)) {}
};

struct UnitExprAST {

};

struct VariableExprAST {
    std::string name;
    VariableExprAST(std::string n): name(std::move(n)) {}
};


template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


#endif //ELLIS_AST_HPP
