#include <string>

#include <gtest/gtest.h>

#include "../src_modgen/tokenize.h"


TEST(TokenizeTest, DerivativeToken) 
{
    std::string src = "dC/dt";

    std::vector<Token> tokens = tokenize(src);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::DERIVATIVE);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_FALSE(tokens[0].value.has_value());
}