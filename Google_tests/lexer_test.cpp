//
// Created by jonathan on 2/15/24.
//
#include "gtest/gtest.h"
#include "lex.hpp"


TEST(LexerTestSuite, SimpleLet) {
    std::string source = "let x = 1;";
    const std::vector<TokenPair> result = {
        TokenPair(tok_let, "let"),
        TokenPair(tok_identifier, "x"),
        TokenPair(tok_operator, "="),
        TokenPair(tok_number, "1"),
        TokenPair(tok_semicolon, ";")
    };
    EXPECT_EQ(lex(source, false), result);
}

TEST(LexerTestSuite, SimpleParen) {
    std::string source = "(x * y + 2)";
    const std::vector<TokenPair> result = {
            TokenPair (tok_lparen, "("),
            TokenPair(tok_identifier, "x"),
            TokenPair(tok_operator, "*"),
            TokenPair(tok_identifier, "y"),
            TokenPair(tok_operator, "+"),
            TokenPair (tok_number, "2"),
            TokenPair (tok_rparen, ")")
    };
    EXPECT_EQ(lex(source, false), result);
}

TEST(LexerTestSuite, SimpleOperator) {
    std::string source = "x += 1";
    const std::vector<TokenPair> result = {
            TokenPair(tok_identifier, "x"),
            TokenPair (tok_operator, "+="),
            TokenPair (tok_number, "1")
    };
    EXPECT_EQ(lex(source, false), result);
}

