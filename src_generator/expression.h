#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "tokenize.h"

class System;

enum class SymbolType 
{
    STATE,
    CONSTANT,
    COMPOUND,
    LIST_INDEX
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
    std::shared_ptr<Expression> negated_expression;

    NegateExpression(std::shared_ptr<Expression> negated_expression)
        : negated_expression(negated_expression)
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
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;

    AddExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs(lhs), rhs(rhs)
    {}

    virtual std::string generate(System& system);
};

class SubtractExpression : public Expression
{
public:
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;

    SubtractExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs(lhs), rhs(rhs)
    {}

    virtual std::string generate(System& system);
};

class MultiplyExpression : public Expression
{
public:
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;

    MultiplyExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs(lhs), rhs(rhs)
    {}

    virtual std::string generate(System& system);
};

class DivideExpression : public Expression
{
public:
    std::shared_ptr<Expression> lhs;
    std::shared_ptr<Expression> rhs;

    DivideExpression(std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
        : lhs(lhs), rhs(rhs)
    {}

    virtual std::string generate(System& system);
};
