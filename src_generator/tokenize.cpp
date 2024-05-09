#include "tokenize.h"

std::string get_token_type_string(TokenType type)
{
    switch (type) 
    {
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::DERIVATIVE: return "DERIVATIVE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::ADD: return "ADD";
        case TokenType::SUBTRACT: return "SUBTRACT";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::ASSIGN: return "ASSIGN";
        default: return "UNKNOWN";
    }
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
    std::smatch matches;

    while (line.size() > 0) {
        
        if (std::regex_search(line, matches, std::regex("^="))) {
            tokens.push_back(Token { TokenType::ASSIGN });
            line = line.substr(1);
            continue;
        }
        
        if (std::regex_search(line, matches, std::regex("^d(.*)/dt"))) {
            std::string symbol = matches[1].str();
            tokens.push_back(Token { TokenType::DERIVATIVE, Symbol(symbol) });
            line = line.substr(matches[0].str().size());
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^\\+"))) {
            tokens.push_back(Token { TokenType::ADD });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^\\-"))) {
            tokens.push_back(Token { TokenType::SUBTRACT });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^\\*"))) {
            tokens.push_back(Token { TokenType::MULTIPLY });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^/"))) {
            tokens.push_back(Token { TokenType::DIVIDE });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^\\("))) {
            tokens.push_back(Token { TokenType::LPAREN });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^\\)"))) {
            tokens.push_back(Token { TokenType::RPAREN });
            line = line.substr(1);
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^([[:alpha:]]+)"))) {
            tokens.push_back(Token { TokenType::SYMBOL, Symbol(matches[0]) });
            line = line.substr(matches[0].str().size());
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^([[:digit:]]+\\.?[[:digit:]]*)"))) {
            tokens.push_back(Token { TokenType::CONSTANT, std::nullopt, std::atof(matches[0].str().c_str()) } );
            line = line.substr(matches[0].str().size());
            continue;
        }

        line = line.substr(1);
    }

    return tokens;
}
