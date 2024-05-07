#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <string>
#include <memory>

#include "expression.h"
#include "tokenize.h"

struct DependentVariable
{
    Symbol symbol;
    float initial_value;
    std::unique_ptr<Expression> rhs;

    DependentVariable(Symbol symbol, std::unique_ptr<Expression> rhs) 
        : symbol(symbol), rhs(std::move(rhs)) {}
};

struct System
{
    std::vector<DependentVariable> dependent_variables;

    SymbolType resolve_symbol_type(Symbol symbol) {
        for (size_t i = 0; i < dependent_variables.size(); ++i)
        {
            if (dependent_variables[i].symbol == symbol) {
                return SymbolType::DEPENDENT;
            }
        }

        return SymbolType::CONSTANT;
    }
};

std::unique_ptr<Expression> parse_expression(System& system, std::vector<Token> tokens);

void parse_dependent_definition(System& system, std::vector<Token> tokens);

void parse_initial_value(System& system, std::vector<Token> tokens);

void parse_declaration(System& system, std::string line);

void parse_system(System& system, std::ifstream& stream);