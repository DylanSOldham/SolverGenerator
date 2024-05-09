#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <string>
#include <memory>
#include <sstream>
#include <regex>

struct Symbol
{
    char main_symbol;
    std::vector<char> subscripts;

    Symbol(std::string sym, std::vector<char> subscripts = {})
        : main_symbol(sym[0]), subscripts(subscripts)
    {}

    std::string to_string()
    {
        std::stringstream code;
        code << main_symbol;
        if (subscripts.size() != 0) {
            code << "_";
            for (char c : subscripts)
                code << c;
        }
        return code.str();
    }

    bool operator==(const Symbol& op)
    {
        if (op.main_symbol != this->main_symbol) return false;
        for (size_t i = 0; i < op.subscripts.size(); ++i) {
            if (op.subscripts[i] != subscripts[i]) return false;
        }

        return true;
    }
};

enum class TokenType {
    CONSTANT,
    SYMBOL,
    DERIVATIVE,
    LPAREN,
    RPAREN,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    ASSIGN
};

std::string get_token_type_string(TokenType type);

struct Token {
    TokenType type;
    std::optional<Symbol> symbol;
    std::optional<double> value;

    std::string to_string() {
        std::stringstream str;

        str << "[" << get_token_type_string(type);
        if (symbol.has_value()) {
            str << ", " << symbol.value().to_string();
        }
        if (value.has_value()) {
            str << ", " << value.value();
        }
        str << "]";

        return str.str();
    }
};

Token tokenize_derivative(std::string& line);

Symbol tokenize_symbol(std::string& line);

double tokenize_constant(std::string& line);

std::vector<Token> tokenize(std::string line);