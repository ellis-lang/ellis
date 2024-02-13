//
// Created by jonathan on 2/12/24.
//

#include "parser.hpp"
#include "lex.hpp"
#include <iostream>


std::unique_ptr<ExprAST> parse_paren_expr(std::vector<TokenPair>& tokens, bool debug=false);
std::unique_ptr<ExprAST> parse_primary(std::vector<TokenPair>& tokens, bool debug=false);


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

std::unique_ptr<ExprAST> parse_expression(std::vector<TokenPair>& tokens, bool debug=false) {
    auto lhs = parse_primary(tokens, debug=debug);

    if (debug) {
        std::cout << "Inside parse_expression, got:\n";
        lhs->print(std::cout);
    }

    if (!lhs) {
        return nullptr;
    }
    return lhs;
}

std::unique_ptr<ExprAST> parse_argument(std::vector<TokenPair>& tokens, bool debug=false) {
    switch (tokens[0].first) {
        case tok_number: {
            auto num = tokens[0].second;
            tokens.erase(tokens.begin());
            if (debug) {
                std::cout << "In parse_argument:\n";
                std::cout << "Found number: " << num << "\n";
            }
            return std::make_unique<NumberExprAST>(NumberExprAST(std::stod(num)));
        }
        case tok_identifier: {
            auto name = tokens[0].second;
            tokens.erase(tokens.begin());
            if (debug) {
                std::cout << "In parse_argument:\n";
                std::cout << "Found identifier: " << name << "\n";
            }
            return std::make_unique<VariableExprAST>(VariableExprAST(name));
        }
        case tok_lparen:
            return parse_paren_expr(tokens);
        default:
            throw std::invalid_argument("Unexpected token while parsing function argument: " + tokens[0].second);
    }
}

std::unique_ptr<ExprAST> parse_identifier_expr(std::vector<TokenPair>& tokens, bool debug=false) {
    auto name = tokens[0].second;
    tokens.erase(tokens.begin());

    // Next token can be:
    // 1. number
    // 2. ident
    // 3. (
    // OR
    // 4. binaryOp
    switch (tokens[0].first) {
        case tok_identifier:
        case tok_number:
        case tok_lparen: {
            std::vector<std::unique_ptr<ExprAST>> arguments;
            while (tokens[0].first != tok_semicolon && tokens[0].first != tok_rparen) {
                arguments.push_back(std::move(parse_argument(tokens)));
            }

            if (debug) {
                std::cout << "Exiting argument parsing loop after finding: " << tokens[0].second << "\n";
            }

            if (tokens[0].first == tok_semicolon)
                tokens.erase(tokens.begin()); // erase ';'
            auto ast = CallExprAST(name, std::move(arguments));
            if (debug) {
                std::cout << "Returning from parse_identifier_expr:\n";
                ast.print(std::cout);
            }

            return std::make_unique<CallExprAST>(std::move(ast));
        }
        default:
            throw std::invalid_argument("Expected ')', found: " + tokens[0].second);
    }
}

std::unique_ptr<ExprAST> parse_number_expr(std::vector<TokenPair>& tokens, bool debug=false) {
    auto ast = std::make_unique<NumberExprAST>(NumberExprAST(std::stod(tokens[0].second)));
    tokens.erase(tokens.begin());
    if (debug) {
        ast->print(std::cout);
        std::cout << "\n";
    }
    return std::move(ast);
}

std::unique_ptr<ExprAST> parse_paren_expr(std::vector<TokenPair>& tokens, bool debug) {
    tokens.erase(tokens.begin());
    if (debug) {
        std::cout << "Inside parse_paren_expr\n";
    }
    auto expr = parse_expression(tokens);

    if (debug) {
        expr->print(std::cout);
    }

    if (!expr) {
        return nullptr;
    }

    if (tokens.empty())
        throw std::invalid_argument("Expected ')', found EOF ");

    if (tokens[0].first != tok_rparen)
        throw std::invalid_argument("Expected ')', found: " + tokens[0].second);

    tokens.erase(tokens.begin());
    return expr;
}

std::unique_ptr<ExprAST> parse_primary(std::vector<TokenPair>& tokens, bool debug) {
    if (debug) {
        std::cout << "Inside parse_primary:\n";
    }
    switch (tokens[0].first) {
        case tok_identifier:
            if (debug) {
                std::cout << "Found identifier, going to parse_identifier_expr\n";
            }
            return parse_identifier_expr(tokens);
        case tok_number:
            if (debug) {
                std::cout << "Found number, going to parse_number_expr\n";
            }
            return parse_number_expr(tokens);
        case tok_lparen:
            if (debug) {
                std::cout << "Found lparen, going to parse_paren_expr\n";
            }
            return parse_paren_expr(tokens, debug);
    }
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
            auto current = std::make_unique<NumberExprAST>(NumberExprAST(std::stod(tokens[0].second)));
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


std::vector<std::unique_ptr<AST>> parse(std::vector<TokenPair>& tokens, bool debug=false) {
    std::cout << "[DEBUG]: "<< debug << "\n";
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
                        ast.push_back(parse_expression(tokens, debug));
                    }
                }
            case tok_number:
                ast.push_back(parse_expression(tokens, debug));
                break;
            case tok_lparen:
                ast.push_back(parse_expression(tokens, debug));
                break;
            default:
                throw std::invalid_argument("Unexpected token: " + current_token.second);
        }
    }
    return ast;
}


FunctionAST parse_function_def(std::vector<TokenPair>& tokens) {

}