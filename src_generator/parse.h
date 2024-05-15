#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <string>
#include <memory>

#include "expression.h"
#include "tokenize.h"

struct StateVariable
{
    Symbol symbol;
    std::shared_ptr<Expression> rhs;

    StateVariable(Symbol symbol, std::shared_ptr<Expression> rhs) 
        : symbol(symbol), rhs(std::move(rhs)) {}
};

struct InitialState
{
    Symbol symbol;
    std::shared_ptr<Expression> rhs;
};

struct System
{
    std::vector<StateVariable> state_variables; // Represents the state, which may or may not include lists
    std::vector<InitialState> initial_states;

    std::map<std::string, size_t> state_lists; // The lists that have been defined
    std::map<std::string, size_t> list_bindings;

    size_t max_index = 0;

    SymbolType resolve_symbol_type(Symbol symbol) {
        if (list_bindings.count(symbol.symbol))
        {
            return SymbolType::LIST_INDEX;
        }

        for (size_t i = 0; i < state_variables.size(); ++i)
        {
            if (state_variables[i].symbol == symbol) {
                return SymbolType::STATE;
            }
        }

        return SymbolType::CONSTANT;
    }
};

std::shared_ptr<Expression> parse_expression(std::vector<Token> tokens);

void parse_state_definition(System& system, std::vector<Token> tokens);

void parse_initial_value(System& system, std::vector<Token> tokens);

void parse_declaration(System& system, std::string line);

void read_system(System& system, std::ifstream& stream);