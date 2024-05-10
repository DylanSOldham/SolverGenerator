#include "tokenize.h"

std::string get_token_type_string(TokenType type)
{
    switch (type) 
    {
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::SYMBOL: return "SYMBOL";
        case TokenType::DERIVATIVE: return "DERIVATIVE";
        case TokenType::LIST: return "LIST";
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

std::vector<Symbol> get_indices(std::string indexList)
{
    std::vector<Symbol> indices;
    std::smatch matches;

    while (indexList.size() > 0) {
        std::regex_search(indexList, matches, std::regex("^,? ?([A-Za-z_]+)"));
        if (matches.size() == 0) break;

        indices.push_back(Symbol(matches[1].str()));
        indexList = indexList.substr(matches[0].str().size());
    }

    return indices;
}

std::vector<float> get_list(std::string listText)
{
    std::vector<float> list;
    std::smatch matches;

    while (listText.size() > 0)
    {
        std::regex_search(listText, matches, std::regex("^,? ?([0-9]+\\.?[0-9]*)"));
        if (matches.size() == 0) break;

        list.push_back(std::atof(matches[1].str().c_str()));
        listText = listText.substr(matches[0].str().size());
    }

    return list;
}

std::vector<Token> tokenize(std::string line) 
{
    std::vector<Token> tokens;
    std::smatch matches;

    while (line.size() > 0) {
        
        if (std::regex_search(line, matches, std::regex("^d(.*)\\((.*)\\)/dt"))) {
            tokens.push_back(Token { TokenType::DERIVATIVE, Symbol(matches[1].str(), get_indices(matches[2].str())), std::nullopt });
            line = line.substr(matches[0].str().size());
            continue;
        }
        
        if (std::regex_search(line, matches, std::regex("^d(.*)/dt"))) {
            std::string symbol = matches[1].str();
            tokens.push_back(Token { TokenType::DERIVATIVE, Symbol(symbol) });
            line = line.substr(matches[0].str().size());
            continue;
        }
        
        if (std::regex_search(line, matches, std::regex("^="))) {
            tokens.push_back(Token { TokenType::ASSIGN });
            line = line.substr(1);
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
        
        if (std::regex_search(line, matches, std::regex("^([A-Za-z_][A-Za-z_0-9]*)\\((.*)\\)"))) {
            tokens.push_back(Token { TokenType::SYMBOL, Symbol(matches[1].str(), get_indices(matches[2].str())) });
            line = line.substr(matches[0].str().size());
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^([A-Za-z_][A-Za-z_0-9]*)"))) {
            tokens.push_back(Token { TokenType::SYMBOL, Symbol(matches[0]) });
            line = line.substr(matches[0].str().size());
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^([0-9]+\\.?[0-9]*), ?(?:([0-9]+\\.?[0-9]*),? ?)+"))) {
            tokens.push_back(Token { TokenType::LIST, std::nullopt, std::nullopt, get_list(matches[0].str()) } );
            line = line.substr(matches[0].str().size());
            continue;
        }

        if (std::regex_search(line, matches, std::regex("^([0-9]+\\.?[0-9]*)"))) {
            tokens.push_back(Token { TokenType::CONSTANT, std::nullopt, std::atof(matches[0].str().c_str()) } );
            line = line.substr(matches[0].str().size());
            continue;
        }

        line = line.substr(1);
    }

    return tokens;
}
