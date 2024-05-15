#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <string>
#include <memory>
#include <sstream>
#include <regex>

enum class IndexType
{
    VARIABLE,
    EXPRESSION
};

class Expression;

struct ListIndex {
    IndexType type;
    std::optional<std::string> list_symbol;
    std::shared_ptr<Expression> expression = nullptr;
    size_t index_start = -1;
    size_t index_end = -2;

    ~ListIndex();
};

struct Symbol
{
    std::string symbol;
    std::vector<ListIndex> indices;

    Symbol(std::string sym)
        : symbol(sym)
    {
    }

    Symbol(std::string sym, std::vector<ListIndex> indices)
        : symbol(sym), indices(indices)
    {
    }

    std::string to_string()
    {
        return symbol;
    }

    bool operator==(const Symbol& op)
    {
        return op.symbol == symbol;
    }

    std::optional<Symbol> uninitial() {
        std::smatch matches;
        if (std::regex_search(symbol, matches, std::regex("(.+)_0$")))
        {
            return Symbol(matches[1], indices);
        }
        return std::nullopt;
    }

    bool is_initial() {
        return std::regex_search(symbol, std::regex("_0$"));
    }

    bool is_list() {
        return indices.size() != 0;
    }
};

enum class TokenType {
    CONSTANT,
    SYMBOL,
    DERIVATIVE,
    LIST,
    LPAREN,
    RPAREN,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    EXPONENT,
    SQRT,
    ASSIGN,
    COMMA
};

std::string get_token_type_string(TokenType type);

struct Token {
    TokenType type;
    std::optional<Symbol> symbol;
    std::optional<float> value; // Used by CONSTANT tokens
    size_t list_size = 0; // Used by LIST tokens
    

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

std::vector<Token> tokenize(std::string line);