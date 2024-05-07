#include <string>

#include <gtest/gtest.h>

#include "../src_generator/tokenize.h"
#include "../src_generator/parse.h"


TEST(Tokenize, DerivativeToken) 
{
    std::string src = "dC/dt";

    std::vector<Token> tokens = tokenize(src);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::DERIVATIVE);

    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_FALSE(tokens[0].value.has_value());

    EXPECT_EQ(tokens[0].symbol.value().main_symbol, 'C');
}

TEST(Parse, parse_dependent_definition)
{
    std::vector<Token> tokens = tokenize("dC/dt = C");

    System system;
    parse_dependent_definition(system, tokens);

    ASSERT_EQ(system.dependent_variables.size(), 1);

    auto& var = system.dependent_variables[0];

    ASSERT_EQ(system.dependent_variables[0].symbol.main_symbol, 'C');
    ASSERT_EQ(typeid(*var.rhs.get()), typeid(SymbolExpression));
}