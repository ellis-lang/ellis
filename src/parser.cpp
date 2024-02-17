//
// Created by jonathan on 2/12/24.
//

#include "parser.hpp"
#include "lex.hpp"
#include <iostream>
#include <cmath>

std::unique_ptr<ExprAST> parse_paren_expr(std::vector<TokenPair>& tokens);
std::unique_ptr<ExprAST> parse_primary(std::vector<TokenPair>& tokens, Token terminator);
std::vector<std::unique_ptr<AST>> parse_body(std::vector<TokenPair>& tokens);

class ParsingException : public std::exception {
    std::string message;
public:
    explicit ParsingException(const std::string& msg) : message(msg) {}
    const char* what () const noexcept override {
        return message.c_str();
    }
};

const std::map<Token, int> infix_op = {
    {tok_aster, 10},
    {tok_plus, 9},
    {tok_minus, 9},
    {tok_equal, 11},
    {tok_greater_than, 8},
    {tok_less_than, 8}
};



bool is_infix(const TokenPair& t) {
    return infix_op.find(t.first) != infix_op.end();
}

std::unique_ptr<ExprAST> parse_expression(std::vector<TokenPair>& tokens, const Token terminator=tok_semicolon) {
    auto lhs = parse_primary(tokens, terminator);
    if (tokens[0].first != terminator) {
        throw ParsingException("Expected terminator at the end of expression, got: " + tokens[0].second );
    }
    tokens.erase(tokens.begin());

    if (!lhs) {
        return nullptr;
    }
    return lhs;
}

std::unique_ptr<ExprAST> parse_argument(std::vector<TokenPair>& tokens) {
    switch (tokens[0].first) {
        case tok_number: {
            const auto num = tokens[0].second;
            tokens.erase(tokens.begin());
            return std::make_unique<NumberExprAST>(NumberExprAST(std::stod(num)));
        }
        case tok_identifier: {
            const auto name = tokens[0].second;
            tokens.erase(tokens.begin());
            return std::make_unique<VariableExprAST>(VariableExprAST(name));
        }
        case tok_string_literal: {
            const auto str = tokens[0].second;
            tokens.erase(tokens.begin());
            return std::make_unique<StringExprAST>(StringExprAST(str));
        }
        case tok_char_literal: {
            const auto ch = tokens[0].second[0];
            tokens.erase(tokens.begin());
            return std::make_unique<CharExprAST>(CharExprAST(ch));
        }
        case tok_lparen:
            return parse_paren_expr(tokens);
        default:
            throw ParsingException("Unexpected token while parsing function argument: " + tokens[0].second);
    }
}

std::unique_ptr<ExprAST> parse_identifier_expr(std::vector<TokenPair>& tokens, const Token terminator=tok_semicolon) {
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
            std::vector<std::unique_ptr<ExprAST>> arguments;
            while (tokens[0].first != terminator) {
                arguments.push_back(std::move(parse_argument(tokens)));
            }

            auto ast = CallExprAST(name, std::move(arguments));
            return std::make_unique<CallExprAST>(std::move(ast));
        }
        default:
            if (tokens[0].first == terminator) {
                return std::make_unique<VariableExprAST>(VariableExprAST(name));
            }
            throw ParsingException("Expected ')', found: " + tokens[0].second);
    }
}

std::unique_ptr<ExprAST> parse_number_expr(std::vector<TokenPair>& tokens, const Token terminator=tok_semicolon) {
    auto ast = std::make_unique<NumberExprAST>(NumberExprAST(std::stod(tokens[0].second)));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<ExprAST> parse_string_expr(std::vector<TokenPair>& tokens) {
    auto ast = std::make_unique<StringExprAST>(StringExprAST(tokens[0].second));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<ExprAST> parse_char_expr(std::vector<TokenPair>& tokens) {
    auto ast = std::make_unique<CharExprAST>(CharExprAST(tokens[0].second[0]));
    tokens.erase(tokens.begin());
    return std::move(ast);
}

std::unique_ptr<ExprAST> parse_paren_expr(std::vector<TokenPair>& tokens) {
    tokens.erase(tokens.begin()); // remove '('
    if (tokens[0].first == tok_rparen) {
        tokens.erase(tokens.begin()); // remove ')'
        return std::make_unique<UnitExprAST>(UnitExprAST());
    }

    auto expr = parse_expression(tokens, tok_rparen);

    if (!expr) {
        return nullptr;
    }

    return expr;
}

std::unique_ptr<ExprAST> parse_primary(std::vector<TokenPair>& tokens, const Token terminator=tok_semicolon) {
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


std::unique_ptr<StatementAST> parse_let(std::vector<TokenPair>& tokens) {

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
        case tok_equal: {
            tokens.erase(tokens.begin()); // remove '='
            auto expr = parse_expression(tokens);
            return std::make_unique<VariableDefAST>(ident, std::move(expr));
        }
        case tok_identifier: {
            auto arg_names = std::vector<std::string>();
            while (tokens[0].first == tok_identifier) {
                arg_names.push_back(tokens[0].second);
                if (tokens.empty())
                    throw ParsingException("Expected '=' before function body");
                tokens.erase(tokens.begin());
            }

            if (tokens[0].first == tok_equal) {
                auto ast = PrototypeAST(ident, arg_names);
                tokens.erase(tokens.begin());
                auto func =  std::make_unique<FunctionAST>(FunctionAST(std::make_unique<PrototypeAST>(ast), parse_body(tokens)));
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
            if (tokens[0].first != tok_equal)
                throw ParsingException("Expected '=' before function body");

            tokens.erase(tokens.begin());
            auto func = std::make_unique<FunctionAST>(FunctionAST(std::make_unique<PrototypeAST>(ast), parse_body(tokens)));
            tokens.erase(tokens.begin());
            return std::move(func);
        }
        default:
            throw ParsingException("Expected identifier, '=' or '(' in let statement, recieved: " + tokens[0].second);
    }
}

std::unique_ptr<IfAST> parse_if(std::vector<TokenPair>& tokens) {
    tokens.erase(tokens.begin()); // remove 'if'
    auto conditional = parse_expression(tokens, tok_then);
    auto body_true = parse_body(tokens);
    if (tokens[0].first == tok_end) {
        tokens.erase(tokens.begin());
        return std::make_unique<IfAST>(IfAST(std::move(conditional), std::move(body_true), {}));
    }

    if (tokens[0].first == tok_else) {
        tokens.erase(tokens.begin());
        auto body_false = parse_body(tokens);
        tokens.erase(tokens.begin());
        return std::make_unique<IfAST>(IfAST(std::move(conditional), std::move(body_true), std::move(body_false)));
    }

    throw ParsingException("Unexpected token while parsing if statement: " + tokens[0].second);
}

std::vector<std::unique_ptr<AST>> parse_body(std::vector<TokenPair>& tokens) {
    std::vector<std::unique_ptr<AST>> ast;
    while (tokens[0].first != tok_end && tokens[0].first != tok_else) {
        auto current_token = tokens[0];
        switch (current_token.first) {
            case tok_let:
                ast.push_back(parse_let(tokens));
            break;
            case tok_identifier:
                // 1. function call
                    // 2. redefinition of variable
                if (tokens.size() > 1) {
                    if (tokens[1].first == tok_equal) {
                        auto name = current_token.second;
                        tokens.erase(tokens.begin(), tokens.begin() + 2);

                        auto node = std::make_unique<BinaryExprAST>("=",
                            std::make_unique<VariableExprAST>(VariableExprAST(name)),
                            parse_expression(tokens));
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


std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair>& tokens) {
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
                    if (tokens[1].first == tok_equal) {
                        auto name = current_token.second;
                        tokens.erase(tokens.begin(), tokens.begin() + 2);

                        auto node = std::make_unique<BinaryExprAST>("=",
                            std::make_unique<VariableExprAST>(VariableExprAST(name)),
                            parse_expression(tokens));
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
        std::cout << *ast.back() << "\n";
    }
    return ast;
}

