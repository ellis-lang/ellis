//
// Created by jon on 4/13/24.
//

#ifndef ELLIS_ABSTRACT_SYNTAX_TREE_H
#define ELLIS_ABSTRACT_SYNTAX_TREE_H

#include <iostream>
#include <llvm/IR/Value.h>


class ASTVisitor;

class AST {
public:
    virtual ~AST() = default;
    virtual void print(std::ostream& stream) const {
        stream << "AST";
    }
    virtual llvm::Value *codegen(ASTVisitor &visitor) = 0;
};

class ExprAST : public AST{
public:
    virtual ~ExprAST() = default;
    void print(std::ostream& stream) const override {
        stream << "ExprAST";
    }
    virtual llvm::Value *codegen(ASTVisitor &visitor) = 0;
};

class StatementAST : public AST {
public:
    virtual ~StatementAST() = default;
    void print(std::ostream& stream) const override {
        stream << "StatementAST";
    }
    virtual void Accept(ASTVisitor& v) = 0;
    virtual llvm::Value* getCode() = 0;
};

class UnitExprAST : public ExprAST {
    llvm::Value* code;
public:
    void print(std::ostream& stream) const override {
        stream << "Unit()";
    }
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
public:
    double val;
    llvm::Value* code;
    explicit NumberExprAST(const double Val) : val(Val) {}

    void print(std::ostream& stream) const override {
        stream << "Number(" << val << ")";
    }
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

class StringExprAST : public ExprAST {
public:
    explicit StringExprAST(std::string Val) : val(std::move(Val)) {}
    void print (std::ostream& stream) const override {
        stream << "String(" << val << ")";
    }
    std::string val;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

class CharExprAST : public ExprAST {
public:
    explicit CharExprAST(char Val) : val(Val) {}
    void print (std::ostream& stream) const override {
        stream << "Char(" << val << ")";
    }
    char val;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

class VariableExprAST : public ExprAST {
public:
    explicit VariableExprAST(std::string Name) : name(std::move(Name)) {}

    void print (std::ostream& stream) const override {
        stream << "Variable(" << name << ")";
    }
    std::string name;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

class VariableDefAST : public StatementAST {
public:
    VariableDefAST(std::string Name, std::unique_ptr<ExprAST> v)
            : name(std::move(Name)), value(std::move(v)) {}

    void print (std::ostream& stream) const override {
        stream << "VariableDef(" << name << " = ";
        value->print(stream);
        stream << ")";
    }
    std::string name;
    std::unique_ptr<ExprAST> value;
    llvm::Value* code;
    llvm::Value* codegen(ASTVisitor &visitor) override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
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
    std::string Op;
    std::unique_ptr<ExprAST> LHS, RHS;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;

};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
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
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;

};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
public:
    PrototypeAST(std::string Name, std::vector<std::string> Args)
            : Name(std::move(Name)), Args(std::move(Args)) {}

    void print(std::ostream& stream) const {
        stream << Name << "( ";
        for (const auto& arg: Args)
            stream << arg << " ";
        stream << ")";
    }
    std::string Name;
    std::vector<std::string> Args;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST : public StatementAST {
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
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<AST>> body;
    llvm::Function* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

class ReturnAST: public StatementAST {
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
    std::unique_ptr<ExprAST> ret;
    llvm::Value* code;
};

class IfAST : public ExprAST {
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
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<AST>> body_on_true;
    std::vector<std::unique_ptr<AST>> body_on_false;
    llvm::Value* code;
    llvm::Value *codegen(ASTVisitor &visitor) override;
};

inline std::ostream& operator<<(std::ostream& os, const AST& a ) {
    a.print(os);
    return os;
}



#endif //ELLIS_ABSTRACT_SYNTAX_TREE_H
