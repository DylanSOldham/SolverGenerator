#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "tokenize.h"

class System;

enum class SymbolType 
{
    STATE,
    COMPOUND,
    LIST_INDEX
};

class Expression
{
public:

    virtual std::string generate(System& system) = 0;
    virtual bool has_state_dependencies(System& system)
    {
        return false;
    }
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
        std::stringstream str;
        str << value;
        return str.str();
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
    virtual bool has_state_dependencies(System& system);
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

    virtual bool has_state_dependencies(System& system)
    {
        return negated_expression->has_state_dependencies(system);
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

    virtual bool has_state_dependencies(System& system)
    {
        return lhs->has_state_dependencies(system) || rhs->has_state_dependencies(system);
    }
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

    virtual bool has_state_dependencies(System& system)
    {
        return lhs->has_state_dependencies(system) || rhs->has_state_dependencies(system);
    }
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

    virtual bool has_state_dependencies(System& system)
    {
        return lhs->has_state_dependencies(system) || rhs->has_state_dependencies(system);
    }
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

    virtual bool has_state_dependencies(System& system)
    {
        return lhs->has_state_dependencies(system) || rhs->has_state_dependencies(system);
    }
};

class ExponentExpression : public Expression
{
public:
    std::shared_ptr<Expression> base;
    std::shared_ptr<Expression> exp;

    ExponentExpression(std::shared_ptr<Expression> base, std::shared_ptr<Expression> exp)
        : base(base), exp(exp)
    {}

    virtual std::string generate(System& system);

    virtual bool has_state_dependencies(System& system)
    {
        return base->has_state_dependencies(system) || exp->has_state_dependencies(system);
    }
};

class SqrtExpression : public Expression
{
public:
    std::shared_ptr<Expression> base;

    SqrtExpression(std::shared_ptr<Expression> base)
        : base(base)
    {}

    virtual std::string generate(System& system);

    virtual bool has_state_dependencies(System& system)
    {
        return base->has_state_dependencies(system);
    }
};
