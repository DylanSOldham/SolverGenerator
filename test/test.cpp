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

TEST(Tokenize, ListToken) 
{
    std::string text = "1 .. 3";
    std::vector<Token> tokens = tokenize(text);

    EXPECT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, TokenType::LIST);
    EXPECT_EQ(tokens[0].list_size, 3);
}

TEST(Tokenize, SqrtToken) 
{
    std::string text = "sqrt(q)";
    std::vector<Token> tokens = tokenize(text);
    
    EXPECT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::SQRT);
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
    std::shared_ptr<Expression> expr = parse_expression(tokens);

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
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(SubtractExpression));

    SubtractExpression& subexpr = *dynamic_cast<SubtractExpression*>(expr.get());
    ASSERT_EQ(typeid(*subexpr.lhs.get()), typeid(SymbolExpression));
    ASSERT_EQ(typeid(*subexpr.rhs.get()), typeid(SymbolExpression));

    SymbolExpression& lhsExpr = *dynamic_cast<SymbolExpression*>(subexpr.lhs.get());
    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(subexpr.rhs.get());
    ASSERT_EQ(lhsExpr.symbol.symbol, std::string("A"));
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
}

TEST(Parse, NegateParentheses)
{
    std::vector<Token> tokens = tokenize("-(A)");
    System system;
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(NegateExpression));

    NegateExpression& negate_expr = *dynamic_cast<NegateExpression*>(expr.get());
    ASSERT_EQ(typeid(*negate_expr.negated_expression.get()), typeid(SymbolExpression));
    SymbolExpression& negatedExpression = *dynamic_cast<SymbolExpression*>(negate_expr.negated_expression.get());
    ASSERT_EQ(negatedExpression.symbol.symbol, std::string("A"));
}

TEST(Parse, TrickyNegate)
{
    std::vector<Token> tokens = tokenize("- A + B");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

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

TEST(Parse, TrickyExponent)
{
    std::vector<Token> tokens = tokenize("- A ^ C(1) + B");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(AddExpression));

    AddExpression& addexpr = *dynamic_cast<AddExpression*>(expr.get());
    ASSERT_EQ(typeid(*addexpr.lhs.get()), typeid(NegateExpression));
    ASSERT_EQ(typeid(*addexpr.rhs.get()), typeid(SymbolExpression));

    NegateExpression& lhsExpr = *dynamic_cast<NegateExpression*>(addexpr.lhs.get());
    ASSERT_EQ(typeid(*lhsExpr.negated_expression.get()), typeid(ExponentExpression));

    ExponentExpression& exponent_expression = *dynamic_cast<ExponentExpression*>(lhsExpr.negated_expression.get());
    ASSERT_EQ(typeid(*exponent_expression.exp.get()), typeid(SymbolExpression));

    SymbolExpression& exp = *dynamic_cast<SymbolExpression*>(exponent_expression.exp.get());
    ASSERT_EQ(exp.symbol.indices.size(), 1);

    SymbolExpression& rhsExpr = *dynamic_cast<SymbolExpression*>(addexpr.rhs.get());
    ASSERT_EQ(rhsExpr.symbol.symbol, std::string("B"));
}

TEST(Parse, OrderOfOperations)
{
    std::vector<Token> tokens = tokenize("A + B * C ^ D * E + F");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(AddExpression));
    AddExpression add_expr_1 = *dynamic_cast<AddExpression*>(expr.get());

    ASSERT_EQ(typeid(*add_expr_1.lhs.get()), typeid(AddExpression));
    AddExpression add_expr_2 = *dynamic_cast<AddExpression*>(add_expr_1.lhs.get());

    ASSERT_EQ(typeid(*add_expr_2.rhs.get()), typeid(MultiplyExpression));
    MultiplyExpression multiply_expr_1 = *dynamic_cast<MultiplyExpression*>(add_expr_2.rhs.get());

    ASSERT_EQ(typeid(*multiply_expr_1.lhs.get()), typeid(MultiplyExpression));
    MultiplyExpression multiply_expr_2 = *dynamic_cast<MultiplyExpression*>(multiply_expr_1.lhs.get());

    ASSERT_EQ(typeid(*multiply_expr_2.rhs.get()), typeid(ExponentExpression));
    ExponentExpression exponent_expr = *dynamic_cast<ExponentExpression*>(multiply_expr_2.rhs.get());
}

TEST(Parse, Sqrt)
{
    std::vector<Token> tokens = tokenize("sqrt(7)");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(SqrtExpression));

    SqrtExpression& sqrtexpr = *dynamic_cast<SqrtExpression*>(expr.get());
    ASSERT_EQ(typeid(*sqrtexpr.base.get()), typeid(ConstantExpression));

    ConstantExpression& constant_expression = *dynamic_cast<ConstantExpression*>(sqrtexpr.base.get());
    ASSERT_EQ(constant_expression.value, 7);
}

TEST(Parse, ScientificNotation)
{
    std::vector<Token> tokens = tokenize("10 ^ -3");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

    ASSERT_EQ(typeid(*expr.get()), typeid(ExponentExpression));

    ExponentExpression& exp_expr = *dynamic_cast<ExponentExpression*>(expr.get());
    ASSERT_EQ(typeid(*exp_expr.base.get()), typeid(ConstantExpression));
    ASSERT_EQ(typeid(*exp_expr.exp.get()), typeid(NegateExpression));

    ConstantExpression& base_expression = *dynamic_cast<ConstantExpression*>(exp_expr.base.get());
    ASSERT_EQ(base_expression.value, 10);

    NegateExpression& exp_expression = *dynamic_cast<NegateExpression*>(exp_expr.exp.get());

    ConstantExpression& negated_expression = *dynamic_cast<ConstantExpression*>(exp_expression.negated_expression.get());
    ASSERT_EQ(negated_expression.value, 3);
}

TEST(Parse, NegateAndDivide)
{
    std::vector<Token> tokens = tokenize("- A / B");
    std::shared_ptr<Expression> expr = parse_expression(tokens);

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

TEST(Parse, IndexedSymbol)
{
    std::vector<Token> tokens = tokenize("INITIAL G(1, n, q+3) = C");

    System system;
    parse_initial_value(system, tokens);

    ASSERT_EQ(system.initial_states.size(), 1);
    ASSERT_EQ(system.initial_states[0].symbol.indices.size(), 3);

    auto& indices = system.initial_states[0].symbol.indices;
    ASSERT_EQ(indices[0].type, IndexType::EXPRESSION);
    ASSERT_EQ(indices[1].type, IndexType::VARIABLE);
    ASSERT_EQ(indices[2].type, IndexType::EXPRESSION);
    ASSERT_TRUE(dynamic_cast<AddExpression*>(indices[2].expression.get()));
}

TEST(Parse, IndexedExpression)
{
    std::vector<Token> tokens = tokenize("dG/dt = -C(1, n, g - 1)");

    System system;
    parse_state_definition(system, tokens);

    ASSERT_EQ(system.state_variables.size(), 1);

    NegateExpression* negated_expr = dynamic_cast<NegateExpression*>(system.state_variables[0].rhs.get());
    ASSERT_TRUE(negated_expr);
    SymbolExpression* sym_expr = dynamic_cast<SymbolExpression*>(negated_expr->negated_expression.get());
    ASSERT_TRUE(sym_expr);
    ASSERT_EQ(sym_expr->symbol.indices.size(), 3);

    auto& indices = sym_expr->symbol.indices;
    ASSERT_EQ(indices[0].type, IndexType::EXPRESSION);
    ASSERT_EQ(indices[1].type, IndexType::VARIABLE);
    ASSERT_EQ(indices[2].type, IndexType::EXPRESSION);
    ASSERT_TRUE(dynamic_cast<SubtractExpression*>(indices[2].expression.get()));
}