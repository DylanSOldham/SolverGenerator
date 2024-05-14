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

    EXPECT_EQ(tokens[0].symbol.value().symbol, std::string("C"));
}

TEST(Tokenize, IndexedDerivativeToken) 
{
    std::string text = "dC(n, x,z)/dt";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::DERIVATIVE);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().symbol, std::string("C"));
    EXPECT_TRUE(tokens[0].symbol.value().is_list());
    EXPECT_EQ(tokens[0].symbol.value().indices.size(), 3);
    EXPECT_EQ(tokens[0].symbol.value().indices[0].list_symbol.value(), std::string("n"));
    EXPECT_EQ(tokens[0].symbol.value().indices[1].list_symbol.value(), std::string("x"));
    EXPECT_EQ(tokens[0].symbol.value().indices[2].list_symbol.value(), std::string("z"));
}

TEST(Tokenize, IndexedSymbolToken0) 
{
    std::string text = "D(q)";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::SYMBOL);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().symbol, std::string("D"));
    EXPECT_TRUE(tokens[0].symbol.value().is_list());
    EXPECT_EQ(tokens[0].symbol.value().indices.size(), 1);
    EXPECT_TRUE(tokens[0].symbol.value().indices[0].type == IndexType::VARIABLE);
    EXPECT_EQ(tokens[0].symbol.value().indices[0].list_symbol.value(), std::string("q"));
}

TEST(Tokenize, IndexedSymbolToken1) 
{
    std::string text = "D(1)";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::SYMBOL);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().symbol, std::string("D"));
    EXPECT_TRUE(tokens[0].symbol.value().is_list());
    EXPECT_EQ(tokens[0].symbol.value().indices.size(), 1);
    EXPECT_TRUE(tokens[0].symbol.value().indices[0].type == IndexType::NUMBER);
    EXPECT_EQ(tokens[0].symbol.value().indices[0].index_start, 1);
}

TEST(Tokenize, InitialIndexedSymbolToken) 
{
    std::string text = "D_0(q)";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::SYMBOL);
    EXPECT_TRUE(tokens[0].symbol.has_value());
    EXPECT_EQ(tokens[0].symbol.value().symbol, std::string("D_0"));
    EXPECT_TRUE(tokens[0].symbol.value().is_list());
    EXPECT_EQ(tokens[0].symbol.value().indices.size(), 1);
    EXPECT_EQ(tokens[0].symbol.value().indices[0].list_symbol.value(), std::string("q"));
}

TEST(Tokenize, ListToken) 
{
    std::string text = "1 .. 3";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::LIST);
    EXPECT_EQ(tokens[0].list_size, 3);
}

TEST(Parse, parse_state_definition)
{
    std::vector<Token> tokens = tokenize("dC/dt = C");

    System system;
    parse_state_definition(system, tokens);

    ASSERT_EQ(system.state_variables.size(), 1);

    auto& var = system.state_variables[0];

    ASSERT_EQ(system.state_variables[0].symbol.symbol, std::string("C"));
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
    ASSERT_EQ(lhsExpr.symbol.symbol, std::string("A"));
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
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
    ASSERT_EQ(lhsExpr.symbol.symbol, std::string("A"));
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
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
    ASSERT_EQ(negatedExpression.symbol.symbol, std::string("A"));

    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(addexpr.rhs.get());
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
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
    ASSERT_EQ(lhsExpr.symbol.symbol, std::string("A"));

    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(divexpr.rhs.get());
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
}