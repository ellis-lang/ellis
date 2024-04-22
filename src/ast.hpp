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

#include "llvm/IR/BasicBlock.h"

using namespace llvm;


class Visitor;

union Code {
    Value* v;
    Function* f;
};

class AST {
public:
    virtual ~AST() = default;
    virtual void print(std::ostream& stream) const {
        stream << "AST";
    }
    virtual void Accept(Visitor& v) = 0;
    virtual Code getCode() = 0;
    friend class Visitor;
};

class ExprAST : public AST{
public:
    virtual ~ExprAST() = default;
    void print(std::ostream& stream) const override {
        stream << "ExprAST";
    }
    virtual void Accept(Visitor& v) = 0;
    virtual Code getCode() = 0;
};

class StatementAST : public AST {
public:
    virtual ~StatementAST() = default;
    void print(std::ostream& stream) const override {
        stream << "StatementAST";
    }
    virtual void Accept(Visitor& v) = 0;
    virtual Code getCode() = 0;
};

class UnitExprAST : public ExprAST {
    Code code;
public:
    void print(std::ostream& stream) const override {
        stream << "Unit()";
    }
    void Accept(Visitor& v) override;
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double val;
    Code code;
public:
    explicit NumberExprAST(const double Val) : val(Val) {}

    void print(std::ostream& stream) const override {
        stream << "Number(" << val << ")";
    }

    void Accept(Visitor& v) override;
    double getVal() { return val; }
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

class StringExprAST : public ExprAST {
    std::string val;
    Code code;
public:
    explicit StringExprAST(std::string Val) : val(std::move(Val)) {}
    void print (std::ostream& stream) const override {
        stream << "String(" << val << ")";
    }
    void Accept(Visitor& v) override;
    std::string getVal() { return val; }
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

class CharExprAST : public ExprAST {
    char val;
    Code code;
public:
    explicit CharExprAST(char Val) : val(Val) {}
    void print (std::ostream& stream) const override {
        stream << "Char(" << val << ")";
    }
    void Accept(Visitor& v) override;
    char getVal() { return val; }
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

class VariableExprAST : public ExprAST {
    std::string name;
    Code code;
public:
    explicit VariableExprAST(std::string Name) : name(std::move(Name)) {}

    void print (std::ostream& stream) const override {
        stream << "Variable(" << name << ")";
    }
    void Accept(Visitor& v) override;
    std::string getName() { return name; }
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

class VariableDefAST : public StatementAST {
    std::string name;
    std::unique_ptr<ExprAST> value;
    Code code;
public:
    VariableDefAST(std::string Name, std::unique_ptr<ExprAST> v)
                    : name(std::move(Name)), value(std::move(v)) {}

    void print (std::ostream& stream) const override {
        stream << "VariableDef(" << name << " = ";
        value->print(stream);
        stream << ")";
    }
    void Accept(Visitor& v) override;
    std::string getName() { return name; }
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }
    ExprAST& getValue() { return *value; }
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    std::string Op;
    std::unique_ptr<ExprAST> LHS, RHS;
    Code code;
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
    void Accept(Visitor& v) override;
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }
    ExprAST& getLHS() { return *LHS; }
    ExprAST& getRHS() { return *RHS; }
    std::string& getOp() { return Op; }
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
    Code code;
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
    void Accept(Visitor& v) override;
    void setCode(Value* c) { code.v = c; }
    std::string getCallee() { return Callee; }
    const std::vector<std::unique_ptr<ExprAST>>& getArgs() { return Args; }
    Code getCode() override { return code; }
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
    Code code;
public:
    PrototypeAST(std::string Name, std::vector<std::string> Args)
        : Name(std::move(Name)), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }

    void print(std::ostream& stream) const {
        stream << Name << "( ";
        for (const auto& arg: Args)
            stream << arg << " ";
        stream << ")";
    }
    void Accept(Visitor& v);
    std::vector<std::string>& getArgs() { return Args; }
    std::string& getName() { return Name; }
    void setCode(Function* f) { code.f = f; }
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public StatementAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<AST>> body;
    Code code;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::vector<std::unique_ptr<AST>> Body)
        : Proto(std::move(Proto)), body(std::move(Body)) {}

    void print(std::ostream& stream) const {
        stream << "Function ";
        Proto->print(stream);
        stream << "\n";
        for (const auto &n : body) {
            stream << "\t";
            n->print(stream);
            stream << "\n";
        }
    }
    void Accept(Visitor& v);
    void setCode(Function* c) { code.f = c; }
    Code getCode() { return code; }
    PrototypeAST& getProto() { return *Proto; }
    std::vector<std::unique_ptr<AST>>& getBody() { return body; }
};

class ReturnAST: public StatementAST {
    std::unique_ptr<ExprAST> ret;
    Code code;
public:
    ReturnAST(std::unique_ptr<ExprAST> returnValue) : ret(std::move(returnValue)) {}
    ReturnAST() {
        ret = nullptr;
    }
    void print(std::ostream& stream) const override {
        stream << "Return(";
        if (ret != nullptr) {
            ret->print(stream);
        }
        stream << ")";
    }
    void Accept(Visitor& v) override;
    void setCode(Value* c) { code.v = c; }
    Code getCode() { return code; }
    ExprAST& getRetExpr() {return *ret; }
};

class IfAST : public StatementAST {
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<AST>> body_on_true;
    std::vector<std::unique_ptr<AST>> body_on_false;
    Code code;
public:
    IfAST(std::unique_ptr<ExprAST> cond,
          std::vector<std::unique_ptr<AST>> BodyOnTrue,
          std::vector<std::unique_ptr<AST>> BodyOnFalse)
        : condition(std::move(cond)), body_on_true(std::move(BodyOnTrue)), body_on_false(std::move(BodyOnFalse)) {}

    void print(std::ostream& stream) const override {
        stream << "If(  ";
        condition->print(stream);
        stream << ") then \n";
        for (const auto &n : body_on_true) {
            stream << "\t";
            n->print(stream);
            stream << "\n";
        }
        stream << " else \n";
        for (const auto &n : body_on_false) {
            stream << "\t";
            n->print(stream);
            stream << "\n";
        }
    }
    void Accept(Visitor& v) override;
    void setCode(Value* c) { code.v = c; }
    Code getCode() override { return code; }

};

inline std::ostream& operator<<(std::ostream& os, const AST& a ) {
    a.print(os);
    return os;
}

inline std::unique_ptr<ExprAST> LogError(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

class Visitor {
public:
    // These are for dispatching on visitor's type.
    virtual void Visit(VariableExprAST& ast) = 0;
    virtual void Visit(NumberExprAST& ast) = 0;
    virtual void Visit(StringExprAST& ast) = 0;
    virtual void Visit(CharExprAST& ast) = 0;
    virtual void Visit(IfAST& ast) = 0;
    virtual void Visit(BinaryExprAST& ast) = 0;
    virtual void Visit(FunctionAST& ast) = 0;
    virtual void Visit(CallExprAST& ast) = 0;
    virtual void Visit(VariableDefAST& ast) = 0;
    virtual void Visit(UnitExprAST& ast) = 0;
    virtual void Visit(ReturnAST& ast) = 0;
    virtual void Visit(PrototypeAST& ast) = 0;
};



#endif //AST_HPP
