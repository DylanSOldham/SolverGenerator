#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "tokenize.h"

class System;

enum class SymbolType 
{
    DEPENDENT,
    CONSTANT,
    COMPOUND
};

class Expression
{
public:
    std::vector<Symbol> present_symbols;

    virtual std::string generate(System& system) = 0;
};

class ConstantExpression : public Expression
{
public:
    float value;

    ConstantExpression(float value)
        : value(value)
    {}

    virtual std::string generate(System& system)
    {
        return std::to_string(value);
    }
};

class SymbolExpression : public Expression
{
public:
    Symbol symbol;

    SymbolExpression(Symbol symbol)
        : symbol(symbol)
    {}

    virtual std::string generate(System& system);
};

class NegateExpression : public Expression
{
public:
    std::unique_ptr<Expression> negated_expression;

    NegateExpression(std::unique_ptr<Expression>&& negated_expression)
        : negated_expression(std::move(negated_expression))
    {}

    virtual std::string generate(System& system)
    {
        std::stringstream code;
        code << "-(" << negated_expression->generate(system) << ")";
        return code.str();
    }
};

class AddExpression : public Expression
{
public:
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;

    AddExpression(std::unique_ptr<Expression>&& lhs, std::unique_ptr<Expression>&& rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs))
    {}

    virtual std::string generate(System& system);
};

class SubtractExpression : public Expression
{
public:
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;

    SubtractExpression(std::unique_ptr<Expression>&& lhs, std::unique_ptr<Expression>&& rhs)
        : lhs(std::move(lhs)), rhs(std::move(rhs))
    {}

    virtual std::string generate(System& system);
};
