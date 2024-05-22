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

struct SystemDeclarations
{
    std::vector<StateVariable> state_variables; // Represents the state, which may or may not include lists
    std::vector<InitialState> initial_states;

    std::vector<Function> function_definitions;
    std::map<std::string, size_t> state_lists; // The lists that have been defined
    std::map<std::string, bool> bound_parameters;

    size_t max_index = 0;

    SymbolType resolve_symbol_type(Symbol symbol) {
        if (bound_parameters.count(symbol.name)) return SymbolType::PARAMETER;

        if (find_function_definition(symbol) != nullptr) {
            return SymbolType::FUNCTION;
        }

        // std::map would probably be better than std::vector
        for (size_t i = 0; i < state_variables.size(); ++i)
        {
            if (state_variables[i].symbol == symbol) {
                return SymbolType::STATE;
            }
        }

        std::cerr << "Error: Undefined symbol " << symbol.name << std::endl;
        return SymbolType::UNRESOLVED;
    }

    Function* find_function_definition(Symbol symbol)
    {
        for (auto& f : function_definitions)
        {
            if (f.symbol == symbol) return &f;
        }

        return nullptr;
    }
};

std::shared_ptr<Expression> parse_expression(std::vector<Token>& tokens);
void parse_state_definition(SystemDeclarations& system, std::vector<Token> tokens);
void parse_initial_value(SystemDeclarations& system, std::vector<Token> tokens);
void parse_declaration(SystemDeclarations& system, std::string line);
void read_system(SystemDeclarations& system, std::ifstream& stream);