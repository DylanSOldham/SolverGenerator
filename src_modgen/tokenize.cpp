#include "tokenize.h"

std::string get_token_type_string(TokenType type)
{
    switch (type) 
    {
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::DERIVATIVE: return "DERIVATIVE";
        case TokenType::ADD: return "ADD";
        case TokenType::SUBTRACT: return "SUBTRACT";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::ASSIGN: return "ASSIGN";
        default: return "UNKNOWN";
    }
}

Token tokenize_derivative(std::string& line)
{
    if (line[0] != 'd' || !std::isalpha(line[1]) || line[2] != '/' || line[3] != 'd' || line[4] != 't') {
        std::cerr << "ERROR: Failed to parse derivative in line: " << line << "\n";
    }

    std::string symbol = line.substr(1, line.find('/') - 1);
    line = line.substr(line.find('/') + 3);

    return Token {TokenType::DERIVATIVE, Symbol(symbol) };
}

Symbol tokenize_symbol(std::string& line)
{

    size_t index = 0;
    bool numbers_allowed = false;
    while ( 1 )
    {
        if (index < line.size()) break;
        if (!numbers_allowed && std::isdigit(line[index])) break;
        if (line[index] == '_') numbers_allowed = true;

        if (!std::isalnum(line[index])) break;

        index++;
    }

    Symbol symbol(line.substr(0, index - 1));

    line = line.substr(index + 1);

    return symbol;
}

double tokenize_constant(std::string& line)
{
    bool point_found = false;
    int index = 0;
    while ( 1 )
    {
        if (index == line.size()) break;

        if (line[index] == '.') {
            if (point_found) break;
            point_found = true;
            ++index;
            continue;
        }

        if (!std::isdigit(line[index])) break;

        ++index;
    }

    double num = std::atof(line.substr(0, index).c_str());

    line = line.substr(index);

    return num;
}

std::vector<Token> tokenize(std::string line) 
{
    std::vector<Token> tokens;

    while (line.size() > 0) {
        if (line[0] == 'd') {
            tokens.push_back(tokenize_derivative(line));
            continue;
        }
        if (line[0] == '=') {
            tokens.push_back(Token { TokenType::ASSIGN });
            line = line.substr(1);
            continue;
        }
        if (line[0] == '+') {
            tokens.push_back(Token { TokenType::ADD });
            line = line.substr(1);
            continue;
        }
        if (line[0] == '-') {
            tokens.push_back(Token { TokenType::SUBTRACT });
            line = line.substr(1);
            continue;
        }
        if (line[0] == '*') {
            tokens.push_back(Token { TokenType::MULTIPLY });
            line = line.substr(1);
            continue;
        }
        if (line[0] == '/') {
            tokens.push_back(Token { TokenType::DIVIDE });
            line = line.substr(1);
            continue;
        }
        if (std::isalpha(line[0])) {
            tokens.push_back(Token { TokenType::SYMBOL, tokenize_symbol(line) });
            continue;
        }
        if (std::isdigit(line[0]) || line[0] == '.') {
            tokens.push_back(Token { TokenType::CONSTANT , std::nullopt, tokenize_constant(line) } );
            continue;
        }

        line = line.substr(1);
    }

    return tokens;
}
