#include <string>

#include <gtest/gtest.h>

#include "../src_generator/tokenize.h"
#include "../src_generator/parse.h"


TEST(Tokenize, DerivativeToken) 
{
    std::string text = "dC/dt";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::DERIVATIVE);

    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_FALSE(tokens[0].value.has_value());

    EXPECT_EQ(tokens[0].symbol.value().main_symbol, 'C');
}

TEST(Tokenize, IndexedDerivativeToken) 
{
    std::string text = "dC(n, x,z)/dt";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::INDEXED_DERIVATIVE);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().main_symbol, 'C');
    EXPECT_TRUE(tokens[0].indices.has_value());
    EXPECT_EQ(tokens[0].indices.value().size(), 3);
    EXPECT_EQ(tokens[0].indices.value()[0].main_symbol, 'n');
    EXPECT_EQ(tokens[0].indices.value()[1].main_symbol, 'x');
    EXPECT_EQ(tokens[0].indices.value()[2].main_symbol, 'z');
}

TEST(Tokenize, IndexedSymbolToken) 
{
    std::string text = "D(q, r,s)";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::INDEXED_SYMBOL);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().main_symbol, 'D');
    EXPECT_TRUE(tokens[0].indices.has_value());
    EXPECT_EQ(tokens[0].indices.value().size(), 3);
    EXPECT_EQ(tokens[0].indices.value()[0].main_symbol, 'q');
    EXPECT_EQ(tokens[0].indices.value()[1].main_symbol, 'r');
    EXPECT_EQ(tokens[0].indices.value()[2].main_symbol, 's');
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

TEST(Parse, AddExpression)
{
    std::vector<Token> tokens = tokenize("A + B");
    System system;
    std::unique_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(AddExpression));

    AddExpression& addexpr = *dynamic_cast<AddExpression*>(expr.get());
    ASSERT_EQ(typeid(*addexpr.lhs.get()), typeid(SymbolExpression));
    ASSERT_EQ(typeid(*addexpr.rhs.get()), typeid(SymbolExpression));

    SymbolExpression& lhsExpr = *dynamic_cast<SymbolExpression*>(addexpr.lhs.get());
    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(addexpr.rhs.get());
    ASSERT_EQ(lhsExpr.symbol.main_symbol, 'A');
    ASSERT_EQ(rhsExpr.symbol.main_symbol, 'B');
}

TEST(Parse, SubtractExpression)
{
    std::vector<Token> tokens = tokenize("A - B");
    System system;
    std::unique_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(SubtractExpression));

    SubtractExpression& subexpr = *dynamic_cast<SubtractExpression*>(expr.get());
    ASSERT_EQ(typeid(*subexpr.lhs.get()), typeid(SymbolExpression));
    ASSERT_EQ(typeid(*subexpr.rhs.get()), typeid(SymbolExpression));

    SymbolExpression& lhsExpr = *dynamic_cast<SymbolExpression*>(subexpr.lhs.get());
    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(subexpr.rhs.get());
    ASSERT_EQ(lhsExpr.symbol.main_symbol, 'A');
    ASSERT_EQ(rhsExpr.symbol.main_symbol, 'B');
}

TEST(Parse, TrickyNegate)
{
    std::vector<Token> tokens = tokenize("- A + B");
    std::unique_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(AddExpression));

    AddExpression& addexpr = *dynamic_cast<AddExpression*>(expr.get());
    ASSERT_EQ(typeid(*addexpr.lhs.get()), typeid(NegateExpression));
    ASSERT_EQ(typeid(*addexpr.rhs.get()), typeid(SymbolExpression));

    NegateExpression& lhsExpr = *dynamic_cast<NegateExpression*>(addexpr.lhs.get());
    ASSERT_EQ(typeid(*lhsExpr.negated_expression.get()), typeid(SymbolExpression));
    SymbolExpression& negatedExpression = *dynamic_cast<SymbolExpression*>(lhsExpr.negated_expression.get());
    ASSERT_EQ(negatedExpression.symbol.main_symbol, 'A');

    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(addexpr.rhs.get());
    ASSERT_EQ(rhsExpr.symbol.main_symbol, 'B');
}

TEST(Parse, NegateAndDivide)
{
    std::vector<Token> tokens = tokenize("- A / B");
    std::unique_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(NegateExpression));

    NegateExpression& negatedexpr = *dynamic_cast<NegateExpression*>(expr.get());

    DivideExpression& divexpr = *dynamic_cast<DivideExpression*>(negatedexpr.negated_expression.get());
    ASSERT_EQ(typeid(*divexpr.lhs.get()), typeid(SymbolExpression));
    ASSERT_EQ(typeid(*divexpr.rhs.get()), typeid(SymbolExpression));

    SymbolExpression& lhsExpr = *dynamic_cast<SymbolExpression*>(divexpr.lhs.get());
    ASSERT_EQ(lhsExpr.symbol.main_symbol, 'A');

    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(divexpr.rhs.get());
    ASSERT_EQ(rhsExpr.symbol.main_symbol, 'B');
}