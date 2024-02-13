//
// Created by jonathan on 12/15/23.
//

#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>


class AST {
public:
    virtual ~AST() = default;
};

class ExprAST : public AST{
public:
    virtual ~ExprAST() = default;
};

class StatementAST : public AST {
public:
    virtual ~StatementAST() = default;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;

public:
    explicit NumberExprAST(const double Val) : Val(Val) {}
};

class StringExprAST : public ExprAST {
    std::string val;
public:
    explicit StringExprAST(std::string Val) : val(std::move(Val)) {}
};

class CharExprAST : public ExprAST {
    char val;
public:
    explicit CharExprAST(char Val) : val(Val) {}
};

class VariableExprAST : public ExprAST {
    std::string Name;
public:
    VariableExprAST(std::string Name) : Name(std::move(Name)) {}
};

class VariableDefAST : public StatementAST {
    std::string name;
    std::unique_ptr<ExprAST> value;
public:
    VariableDefAST(const std::string &Name, std::unique_ptr<ExprAST> v)
                    : name(Name), value(std::move(v)) {}
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    std::string Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(std::string Op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

public:
    PrototypeAST(std::string Name, std::vector<std::string> Args)
        : Name(std::move(Name)), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public StatementAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<AST>> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::vector<std::unique_ptr<AST>> Body)
        : Proto(std::move(Proto)), body(std::move(Body)) {}
};

#endif //AST_HPP
