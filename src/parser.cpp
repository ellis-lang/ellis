//
// Created by jonathan on 2/12/24.
//

#include "parser.hpp"
#include "lex.hpp"
#include "print.hpp"
#include <iostream>
#include <cmath>

std::unique_ptr<AST> parse_paren_expr(std::vector<TokenPair> &tokens);

std::unique_ptr<AST> parse_primary(std::vector<TokenPair> &tokens, Token terminator);

std::vector<std::unique_ptr<AST>> parse_body(std::vector<TokenPair> &tokens);

std::unique_ptr<AST>
parse_bin_op_rhs(std::vector<TokenPair> &tokens, int ExprPrec, std::unique_ptr<AST> LHS, Token terminator);


const std::map<std::string, int> BinopPrecedence = {
        {">", 10},
        {"<", 10},
        {"+", 20},
        {"-", 20},
        {"*", 30},
        {"/", 30},
        {"^", 40},
};


std::unique_ptr<AST> make(AST ast) { return make(std::move(ast)); }


std::unique_ptr<AST> parse_expression(std::vector<TokenPair> &tokens, const Token terminator = tok_semicolon) {
    auto lhs = parse_primary(tokens, terminator);
    if (tokens[0].first == terminator) {
        tokens.erase(tokens.begin());
        return lhs;
    } else {
        // infix operator
        if (tokens[0].first == tok_operator) {
            lhs = parse_bin_op_rhs(tokens, 0, std::move(lhs), terminator);
        }
    }

    if (tokens[0].first != terminator) {
        throw ParsingException("Expected terminator at the end of expression, got: " + tokens[0].second);
    }
    tokens.erase(tokens.begin());

    if (!lhs) {
        return nullptr;
    }
    return lhs;
}

std::unique_ptr<AST> parse_argument(std::vector<TokenPair> &tokens) {
    switch (tokens[0].first) {
        case tok_number: {
            const auto num = tokens[0].second;
            tokens.erase(tokens.begin());
            return make(NumberExprAST(std::stod(num)));
        }
        case tok_identifier: {
            const auto name = tokens[0].second;
            tokens.erase(tokens.begin());
            return make(VariableExprAST(name));
        }
        case tok_string_literal: {
            const auto str = tokens[0].second;
            tokens.erase(tokens.begin());
            return make(StringExprAST(str));
        }
        case tok_char_literal: {
            const auto ch = tokens[0].second[0];
            tokens.erase(tokens.begin());
            return make(CharExprAST(ch));
        }
        case tok_lparen:
            return parse_paren_expr(tokens);
        default:
            throw ParsingException("Unexpected token while parsing function argument: " + tokens[0].second);
    }
}

std::unique_ptr<AST> parse_identifier_expr(std::vector<TokenPair> &tokens, const Token terminator = tok_semicolon) {
    auto name = tokens[0].second;
    tokens.erase(tokens.begin()); // remove 'name'

    // Next token can be:
    // 1. number
    // 2. ident
    // 3. (
    // OR
    // 4. binaryOp
    switch (tokens[0].first) {
        case tok_identifier:
        case tok_char_literal:
        case tok_string_literal:
        case tok_number:
        case tok_lparen: {
            std::vector<std::unique_ptr<AST>> arguments;
            while (tokens[0].first != terminator) {
                arguments.push_back(std::move(parse_argument(tokens)));
            }

            auto ast = CallExprAST(name, std::move(arguments));
            return make(std::move(ast));
        }
        case tok_operator: {
            auto lhs = make(VariableExprAST(name));
            auto rhs = parse_bin_op_rhs(tokens, 0, std::move(lhs), terminator);
            return std::move(rhs);
        }
        default:
            if (tokens[0].first == terminator) {
                return make(VariableExprAST(name));
            }
            throw ParsingException("Expected " + TOKEN_STRINGS.at(terminator) + ", found: " + tokens[0].second);
    }
}

std::unique_ptr<AST> parse_number_expr(std::vector<TokenPair> &tokens, const Token terminator = tok_semicolon) {
    auto ast = make(NumberExprAST(std::stod(tokens[0].second)));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<AST> parse_string_expr(std::vector<TokenPair> &tokens) {
    auto ast = make(StringExprAST(tokens[0].second));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<AST> parse_char_expr(std::vector<TokenPair> &tokens) {
    auto ast = make(CharExprAST(tokens[0].second[0]));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<AST> parse_paren_expr(std::vector<TokenPair> &tokens) {
    tokens.erase(tokens.begin()); // remove '('
    if (tokens[0].first == tok_rparen) {
        tokens.erase(tokens.begin()); // remove ')'
        return make(UnitExprAST());
    }
    auto expr = parse_expression(tokens, tok_rparen);
    if (!expr) {
        return nullptr;
    }

    return expr;
}

std::unique_ptr<AST> parse_primary(std::vector<TokenPair> &tokens, const Token terminator = tok_semicolon) {
    switch (tokens[0].first) {
        case tok_identifier:
            return parse_identifier_expr(tokens, terminator);
        case tok_number:
            return parse_number_expr(tokens);
        case tok_char_literal:
            return parse_char_expr(tokens);
        case tok_string_literal:
            return parse_string_expr(tokens);
        case tok_lparen:
            return parse_paren_expr(tokens);
        default:
            throw ParsingException("Unexpected token when parsing primary expression: " + tokens[0].second);
    }
}

int get_tok_precedence(const TokenPair &t) {
    // Make sure it's a declared binop.
    int TokPrec = BinopPrecedence.at(t.second);
    if (TokPrec <= 0) return -1;
    return TokPrec;
}

std::unique_ptr<AST> parse_bin_op_rhs(std::vector<TokenPair> &tokens, int ExprPrec, std::unique_ptr<AST> LHS,
                                      const Token terminator) {
    // If this is a binop, find its precedence.
    while (true) {
        if (tokens[0].first == terminator)
            return LHS;

        int TokPrec = get_tok_precedence(tokens[0]);

        // If this is a binop that binds at least as tightly as the current binop,
        // consume it, otherwise we are done.
        if (TokPrec < ExprPrec) {
            std::cout << "Did not bind tightly enough\n";
            return LHS;
        }

        // Okay, we know this is a binop.
        std::string BinOp = tokens[0].second;
        tokens.erase(tokens.begin());

        // Parse the primary expression after the binary operator.
        auto RHS = parse_primary(tokens, terminator);
        if (!RHS)
            return nullptr;

        if (tokens[0].first == terminator)
            return make(BinaryExprAST(BinOp, std::move(LHS), std::move(RHS)));

        // If BinOp binds less tightly with RHS than the operator after RHS, let
        // the pending operator take RHS as its LHS.
        int NextPrec = get_tok_precedence(tokens[0]);
        if (TokPrec < NextPrec) {
            RHS = parse_bin_op_rhs(tokens, TokPrec + 1, std::move(RHS), terminator);
            if (!RHS)
                return nullptr;
        }

        // Merge LHS/RHS.
        LHS = make(BinaryExprAST(BinOp, std::move(LHS), std::move(RHS)));
    }
}

std::unique_ptr<AST> parse_let(std::vector<TokenPair> &tokens) {

    // remove LET
    tokens.erase(tokens.begin());
    if (tokens.empty()) {
        throw ParsingException("Expected identifier after let statement");
    }

    if (tokens[0].first != tok_identifier) {
        throw ParsingException("Expected identifier after let, got " + tokens[0].second);
    }

    const auto ident = tokens[0].second;
    // remove variable name
    tokens.erase(tokens.begin());

    switch (tokens[0].first) {
        case tok_operator: {
            if (tokens[0].second != "=") {
                throw ParsingException("Unexpected operator in let statement: " + tokens[0].second);
            }
            tokens.erase(tokens.begin()); // remove '='
            auto expr = parse_expression(tokens);
            return make(LetExprAST(ident, std::move(expr)));
        }
        case tok_identifier: {
            auto arg_names = std::vector<std::string>();
            while (tokens[0].first == tok_identifier) {
                arg_names.push_back(tokens[0].second);
                if (tokens.empty())
                    throw ParsingException("Expected '=' before function body");
                tokens.erase(tokens.begin());
            }

            if (tokens[0].second == "=") {
                auto ast = PrototypeAST(ident, arg_names);
                tokens.erase(tokens.begin());
                auto func = make(FunctionAST(std::make_unique<PrototypeAST>(ast), parse_body(tokens)));
                tokens.erase(tokens.begin()); // remove 'end'
                return std::move(func);
            }
            throw ParsingException("Expected '=' before function body");
        }
        case tok_lparen: {
            tokens.erase(tokens.begin());
            if (tokens[0].first != tok_rparen) {
                throw ParsingException("Expected closing ')' in unit function");
            }
            auto ast = PrototypeAST(ident, std::vector<std::string>());
            tokens.erase(tokens.begin());
            if (tokens[0].second != "=")
                throw ParsingException("Expected '=' before function body");

            tokens.erase(tokens.begin());
            auto func = make(FunctionAST(std::make_unique<PrototypeAST>(ast), parse_body(tokens)));
            tokens.erase(tokens.begin());
            return std::move(func);
        }
        default:
            throw ParsingException("Expected identifier, '=' or '(' in let statement, received: " + tokens[0].second);
    }
}

std::unique_ptr<AST> parse_if(std::vector<TokenPair> &tokens) {
    tokens.erase(tokens.begin()); // remove 'if'
    auto conditional = parse_expression(tokens, tok_then);
    auto body_true = parse_body(tokens);
    if (tokens[0].first == tok_end) {
        tokens.erase(tokens.begin());
        return make(IfAST(std::move(conditional), std::move(body_true), {}));
    }

    if (tokens[0].first == tok_else) {
        tokens.erase(tokens.begin());
        auto body_false = parse_body(tokens);
        tokens.erase(tokens.begin());
        return make(IfAST(std::move(conditional), std::move(body_true), std::move(body_false)));
    }

    throw ParsingException("Unexpected token while parsing if statement: " + tokens[0].second);
}

std::vector<std::unique_ptr<AST>> parse_body(std::vector<TokenPair> &tokens) {
    std::vector<std::unique_ptr<AST>> ast;
    while (tokens[0].first != tok_end && tokens[0].first != tok_else) {
        auto current_token = tokens[0];
        switch (current_token.first) {
            case tok_let:
                ast.push_back(parse_let(tokens));
                break;
            case tok_return:
                tokens.erase(tokens.begin()); // remove 'return'
                if (tokens.size() > 1) {
                    if (tokens[0].first == tok_semicolon)
                        ast.push_back(make(ReturnAST()));
                    else
                        ast.push_back(make(ReturnAST(parse_expression(tokens))));
                } else {
                    throw ParsingException("Unexpected end of function definition");
                }
            case tok_identifier:
                // 1. function call
                // 2. redefinition of variable
                if (tokens.size() > 1) {
                    if (tokens[1].second == "=") {
                        auto name = current_token.second;
                        tokens.erase(tokens.begin(), tokens.begin() + 2);

                        auto node = make(BinaryExprAST("=",
                                                       make(VariableExprAST(name)),
                                                       parse_expression(tokens)));
                        ast.push_back(std::move(node));
                    } else {
                        ast.push_back(parse_expression(tokens));
                    }
                }
                break;
            case tok_number:
            case tok_char_literal:
            case tok_string_literal:
            case tok_lparen:
                ast.push_back(parse_expression(tokens));
                break;
            case tok_if:
                ast.push_back(parse_if(tokens));
                break;
            default:
                throw ParsingException("Unexpected token: " + current_token.second);

        }
        if (tokens.empty())
            throw ParsingException("Expected 'end' at end of function definition, got EOF");
    }
    return ast;
}


std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair> &tokens) {
    std::vector<std::unique_ptr<AST>> ast;
    while (!tokens.empty()) {
        auto current_token = tokens[0];
        switch (current_token.first) {
            case tok_let:
                ast.push_back(parse_let(tokens));
                break;
            case tok_identifier:
                // 1. function call
                // 2. redefinition of variable
                if (tokens.size() > 1) {
                    if (tokens[1].second == "=") {
                        auto name = current_token.second;
                        tokens.erase(tokens.begin(), tokens.begin() + 2);

                        auto node = make(
                                BinaryExprAST("=",
                                              make(VariableExprAST(name)),
                                              parse_expression(tokens)));
                        ast.push_back(std::move(node));
                    } else {
                        ast.push_back(parse_expression(tokens));
                    }
                }

                break;
            case tok_number:
            case tok_char_literal:
            case tok_string_literal:
            case tok_lparen:
                ast.push_back(parse_expression(tokens));
                break;
            case tok_if:
                ast.push_back(parse_if(tokens));
                break;
            case tok_return:
                throw ParsingException("'return' statement found outside function definition");
            default:
                throw ParsingException("Unexpected token: " + current_token.second);
        }
        print(*ast.back(), std::cout);
    }
    return ast;
}

