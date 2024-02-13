//
// Created by jonathan on 12/15/23.
//

#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>


class AST {
public:
    virtual ~AST() = default;
    virtual void print(std::ostream& stream) const {
        stream << "AST";
    }
};

class ExprAST : public AST{
public:
    virtual ~ExprAST() = default;
    void print(std::ostream& stream) const override {
        stream << "ExprAST";
    }
};

class StatementAST : public AST {
public:
    virtual ~StatementAST() = default;
    void print(std::ostream& stream) const override {
        stream << "StatementAST";
    }
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;

public:
    explicit NumberExprAST(const double Val) : Val(Val) {}

    void print(std::ostream& stream) const override {
        stream << "Number(" << Val << ")";
    }
};

class StringExprAST : public ExprAST {
    std::string val;
public:
    explicit StringExprAST(std::string Val) : val(std::move(Val)) {}
    void print (std::ostream& stream) const override {
        stream << "String(" << val << ")" << " \n";
    }
};

class CharExprAST : public ExprAST {
    char val;
public:
    explicit CharExprAST(char Val) : val(Val) {}
    void print (std::ostream& stream) const override {
        stream << "Char(" << val << ")" << " \n";
    }
};

class VariableExprAST : public ExprAST {
    std::string Name;
public:
    explicit VariableExprAST(std::string Name) : Name(std::move(Name)) {}

    void print (std::ostream& stream) const override {
        stream << "Variable(" << Name << ")" << " \n";
    }
};

class VariableDefAST : public StatementAST {
    std::string name;
    std::unique_ptr<ExprAST> value;
public:
    VariableDefAST(std::string Name, std::unique_ptr<ExprAST> v)
                    : name(std::move(Name)), value(std::move(v)) {}

    void print (std::ostream& stream) const override {
        stream << "VariableDef(" << name << " = ";
        value->print(stream);
    }
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    std::string Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(std::string Op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(std::move(Op)), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    void print (std::ostream& stream) const override {
        stream << Op << "(";
        LHS->print(stream);

        stream << " , ";
        RHS->print(stream);
        stream << ")";
    }
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(std::string Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(std::move(Callee)), Args(std::move(Args)) {}

    void print (std::ostream& stream) const override {
        stream << Callee << "( ";

        for (const auto& arg : Args) {
            arg->print(stream);
            stream << " ";
        }
        stream << ")";
    }
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

    void print(std::ostream& stream) const {
        stream << Name << " ";
        for (const auto& arg: Args)
            stream << arg;
    }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public StatementAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<AST>> body;

public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::vector<std::unique_ptr<AST>> Body)
        : Proto(std::move(Proto)), body(std::move(Body)) {}

    void print(std::ostream& stream) const override {
        stream << "Function ";
        Proto->print(stream);
    }
};

inline std::ostream& operator<<(std::ostream& os, const AST& a ) {
    a.print(os);
    return os;
}



#endif //AST_HPP
