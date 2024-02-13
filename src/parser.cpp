//
// Created by jonathan on 2/12/24.
//

#include "parser.hpp"
#include "lex.hpp"
#include <iostream>


const std::set<Token> infix_op = {
    tok_aster,
    tok_plus,
    tok_minus,
    tok_equal,
    tok_greater_than,
    tok_less_than
};

bool is_infix(const TokenPair& t) {
    return infix_op.find(t.first) != infix_op.end();
}

std::unique_ptr<ExprAST> parse_expr(std::vector<TokenPair>& tokens) {
    /*
     * This only takes a look at the first two tokens:
     *
     */
    switch (tokens[0].first) {
        case tok_semicolon:
            return nullptr;
            break;
        case tok_number: {
            auto current = std::make_unique<ExprAST>(NumberExprAST(std::stod(tokens[0].second)));
            tokens.erase(tokens.begin());
            if (tokens.size() > 1) {
                if (is_infix(tokens[0])) {
                    auto op = tokens[0].second;
                    tokens.erase(tokens.begin());
                    return std::make_unique<BinaryExprAST>(op, std::move(current), std::move(parse_expr(tokens)));
                }
            } else {
                if (tokens[0].first != tok_semicolon)
                    throw std::invalid_argument("Expected semicolon at end of statement, encountered EOF");
                tokens.erase(tokens.begin());
                return current;
            }
            break;
        }
        default:
            throw std::invalid_argument("Unexpected token encountered while parsing expression: " + tokens[0].second);

    }
}




std::unique_ptr<FunctionAST> parse_function_def(std::vector<TokenPair>& tokens, const std::string& ident) {

}

std::unique_ptr<VariableDefAST> parse_variable_def(std::vector<TokenPair>& tokens, const std::string& ident) {
    return std::make_unique<VariableDefAST>(VariableDefAST(ident, parse_expr(tokens)));
}



std::unique_ptr<StatementAST> parse_let(std::vector<TokenPair>& tokens) {

    // remove LET
    tokens.erase(tokens.begin());

    if (tokens.empty()) {
        throw std::invalid_argument("Expected identifier after let statement");
    }

    if (tokens[0].first != tok_identifier) {
        throw std::invalid_argument("Expected identifier after let, got " + tokens[0].second);
    }

    const auto ident = tokens[0].second;
    tokens.erase(tokens.begin());

    switch (tokens[0].first) {
        case tok_equal:
            return parse_variable_def(tokens, ident);
        case tok_identifier:
            return parse_function_def(tokens, ident);
        case tok_lparen:
            return parse_function_def(tokens, ident);
        default:
            throw std::invalid_argument("Expected identifier, '=' or '(' in let statement, recieved: " + tokens[0].second);

    }

}


std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair>& tokens, bool verbose) {
    std::vector<std::unique_ptr<AST>> ast;
    while (!tokens.empty()) {
        auto current_token = tokens[0];
        switch (current_token.first) {
            case tok_let:
                ast.push_back(parse_let(tokens));
            case tok_identifier:
                // 1. function call
                // 2. redefinition of variable
                if (tokens.size() > 1) {
                    if (tokens[1].first == tok_equal) {
                        ast.push_back(parse_expr(tokens));
                    }
                }
            case tok_number:
                if (verbose)
                    std::cout << "Found number, using parse_expr\n";
                ast.push_back(parse_expr(tokens));
                break;
            case tok_lparen:
                ast.push_back(parse_expr(tokens));
                break;
            default:
                throw std::invalid_argument("Unexpected token: " + current_token.second);
        }
    }
    return ast;
}


FunctionAST parse_function_def(std::vector<TokenPair>& tokens) {

}