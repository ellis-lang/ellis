//
// Created by jonathan on 2/15/24.
//
#include "gtest/gtest.h"
#include "lex.hpp"


TEST(LexerTestSuite, Example1) {
    std::string source = "let x = 1;";
    const std::vector<TokenPair> result = {
        TokenPair(tok_let, "let"),
        TokenPair(tok_identifier, "x"),
        TokenPair(tok_equal, "="),
        TokenPair(tok_number, "1"),
        TokenPair(tok_semicolon, ";")
    };
    EXPECT_EQ(lex(source, false), result);
}

